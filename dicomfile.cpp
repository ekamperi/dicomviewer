#include <sstream>
#include <QDebug>
#include <QFile>
#include <QtGlobal>
#include <QXmlStreamReader>

//#include "Magick++.h"

#include "dicomfile.h"
#include "include/helpers/dicomhelper.h"
#include "include/helpers/huconverter.h"

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcpxitem.h"

#include "dcmtk/dcmimgle/dcmimage.h"

DicomFile::DicomFile()
{
    /* Allocate memory for list */
    this->pList = new QList< QMap<QString, QString> >();
    Q_ASSERT(this->pList);

    this->pHUConverter = new HUConverter();
    Q_ASSERT(this->pHUConverter);

    //this->pRawBlob = NULL;
}

DicomFile::~DicomFile()
{
    Q_ASSERT(this->pList);
    delete pList;

    Q_ASSERT(this->pHUConverter);
    delete this->pHUConverter;

//    if (this->pRawBlob) {
//        delete this->pRawBlob;
//    }
}

void DicomFile::loadDicomFile(QString filename)
{
    this->filename = filename;
    OFCondition status = this->dcmFileFormat.loadFile(filename.toStdString().c_str());
    if (!status.good()) {
        qDebug() << status.text();
    }
    this->pDcmDataset = this->dcmFileFormat.getDataset();

    /*
     * Extract rescale slope and intercept values which are necessary
     * for converting CT intensity values to HUs (Hounsfield Units):
     * HU = pixelValue * slope + intercept
     */
    Float64 rescaleSlope = 0.0;
    Float64 rescaleIntercept = 0.0;
    if (pDcmDataset->findAndGetFloat64(DCM_RescaleSlope, rescaleSlope).good() &&
        pDcmDataset->findAndGetFloat64(DCM_RescaleIntercept, rescaleIntercept).good()) {
    }

    this->pHUConverter->setSlopeIntercept(rescaleSlope, rescaleIntercept);

    /*
     * Extract pixel spacing: 10.7.1.3 Pixel Spacing Value Order and Valid Values.
     *
     * The first value is the row spacing in mm, that is the spacing between the centers
     * of adjacent rows, or vertical spacing. The second value is the column spacing in mm,
     * that is the spacing between the centers of adjacent columns, or horizontal spacing.
     */
    OFString ofsPixelSpacing;
    status = this->pDcmDataset->findAndGetOFStringArray(DCM_PixelSpacing, ofsPixelSpacing);
    Q_ASSERT(status.good());

    /* The values are delimited with an '\' */
    char delim;
    std::stringstream ss(ofsPixelSpacing.c_str());
    ss >> this->vSpacing >> delim >> this->hSpacing;
}

void DicomFile::parseDicomFile(QString filename)
{
    DcmFileFormat dcmFile;
    OFCondition status = dcmFile.loadFile(filename.toStdString().c_str());
    if (!status.good()) {
        qDebug() << status.text();
        return;
    }

    /* Iterate over all objects */
    DcmDataset *pDcmDataset = dcmFile.getDataset();
    DcmStack stack;
    DcmObject *obj = NULL;

    int i = 0;
    while (pDcmDataset->nextObject(stack, /* Depth-first */ OFTrue).good()) {
        obj = stack.top();
        std::stringstream ss;
        obj->writeXML(ss);
        this->pList->append(parseDicomFromXml(ss.str().c_str()));
        i++;
    }

//    i = 0;
//    while (!this->list->isEmpty()) {
//        QMap<QString, QString> e = this->list->takeFirst();
//        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(e["name"]));
//        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(e["tag"]));
//        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(e["len"]));
//        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(e["vr"]));
//        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(e["text"]));
//        i++;
//    }
}

QMap<QString, QString> DicomFile::parseDicomFromXml(const char *s)
{
    QXmlStreamReader xml(s);
    QMap<QString, QString> dicomObject;

    while (!xml.atEnd() && !xml.hasError()) {
        /* Read next element */
        QXmlStreamReader::TokenType token = xml.readNext();

        /* If token is just StartDocument, skip it */
        if (token == QXmlStreamReader::StartDocument) {
            continue;
        }

        /* If token is element, read it */
        if (token == QXmlStreamReader::StartElement) {
            QXmlStreamAttributes attrs = xml.attributes();
            for (int i = 0; i < attrs.size(); i++) {
                dicomObject.insert(
                            attrs.at(i).name().toString(),
                            attrs.at(i).value().toString());
            }
            dicomObject.insert("text", xml.readElementText());
        }
    }
    return dicomObject;
}

float *DicomFile::getUncompressedData()
{
    DicomImage *pDicomImage = new DicomImage(this->filename.toStdString().c_str());
    if ((pDicomImage == NULL) || (pDicomImage->getStatus() != EIS_Normal)) {
        qDebug() << pDicomImage->getStatus();
        return NULL;
    }

    this->cols = pDicomImage->getWidth();
    this->rows = pDicomImage->getHeight();
    this->format = GL_LUMINANCE;

    /*
     * Extract the current windo center/width (units?)
     * C.11.2.1.2.1 says that Window Center and Window Width
     * specify a linear conversion from stored pixels after ... or
     * Rescale Slope and Intercept specified. Thus, units are probably HUs.
     */
    double currWindowCenter;
    double currWindowWidth ;
    pDicomImage->setWindow(0);
    pDicomImage->getWindow(currWindowCenter, currWindowWidth);
    this->pHUConverter->setDefaultCenterWidth(currWindowCenter, currWindowWidth);

    if (pDicomImage->isMonochrome()) {
        /* Extract the 16bit raw pixel data */
        const Uint16 *rawPixel;
        unsigned long cnt;
        OFCondition cond =
                pDcmDataset->findAndGetUint16Array(
                    DCM_PixelData, rawPixel, &cnt, OFFalse);
        Q_ASSERT(cond.good());
        Q_ASSERT(cnt > 0);

        /* Extract the useful 12bit worth of data (0-11bit, Little Endian)
         * XXX: number of bits used and endianness must be extracted from .dcm file
         */
        float *pixels = (float *) malloc(cnt * sizeof(float));
        Q_ASSERT(pixels);

        this->maxPixel = 0;
        for (unsigned long i = 0; i < cnt; i++) {
            Uint16 pixel = rawPixel[i] & 0x0FFF;
            if (pixel > this->maxPixel) {
                this->maxPixel = pixel;
            }
            pixels[i] = (float)pixel;
        }
        Q_ASSERT(this->maxPixel > 0.0);

        return pixels;
    } else {
        qDebug() << "Image is NOT monochrome! We will return NULL!";
    }
    return NULL;
}

unsigned char *DicomFile::getCompressedData()
{
    DcmFileFormat dcmFile;
    OFCondition status = dcmFile.loadFile(this->filename.toStdString().c_str());
    if (!status.good()) {
        qDebug() << status.text();
        return NULL;
    }

    DcmDataset *pDcmDataset = dcmFile.getDataset();
    status = EC_Normal;
    DcmElement* element = NULL;

    status = pDcmDataset->findAndGetElement(DCM_PixelData, element);
    if (status.bad() || element == NULL) {
        qDebug() << status.text();
        return NULL;
    }

    DcmPixelData *dpix = NULL;
    dpix = OFstatic_cast(DcmPixelData*, element);

    /* Since we have compressed data, we must utilize DcmPixelSequence
     * in order to access it in raw format, e. g. for decompressing it
     * with an external library.
     */
    DcmPixelSequence *dseq = NULL;
    E_TransferSyntax xferSyntax = EXS_Unknown;
    const DcmRepresentationParameter *rep = NULL;

    /*
     * Find the key that is needed to access the right representation
     * of the data within DCMTK.
     */
    dpix->getOriginalRepresentationKey(xferSyntax, rep);

    /*
     * Access original data representation and get result within
     * pixel sequence.
     */
    status = dpix->getEncapsulatedRepresentation(xferSyntax, rep, dseq);
    if (status == EC_Normal) {
        DcmPixelItem* pixitem = NULL;

		/* Access first frame (skipping offset table) */
        dseq->getItem(pixitem, 1);
        if (pixitem == NULL)
            return NULL;

        /*
         * Get the length of this pixel item (i.e. fragment, i.e. most of
         * the time, the length of the frame).
         */
		Uint32 length = pixitem->getLength();
        if (length == 0) {
            qDebug() << "length = 0";
            return NULL;
        }

        /* Finally, get the compressed data for this pixel item */
        Uint8 *pixData = NULL;
        status = pixitem->getUint8Array(pixData);
        if (status.bad()) {
            qDebug() << status.text();
            return NULL;
        }

        /* Convert jpeg2000 to png */
        return jp2k_to_png(pixData, length);
    }

    return NULL;
}

unsigned char *DicomFile::jp2k_to_png(Uint8* pixelData, Uint32 length)
{
//    Q_ASSERT(pixelData != NULL);
//    Q_ASSERT(length > 0);

//    this->pRawBlob = new Magick::Blob(pixelData, sizeof(Uint8) * length);
//    Magick::Image image;
//    try {
//        image.magick("RGB");
//        image.read(*this->pRawBlob);

//        this->rows = image.rows();
//        this->cols = image.columns();
//        this->format = GL_RGB;

//        qDebug() << "columns =" << image.columns()
//                 << "rows =" << image.rows();
//    } catch (Magick::Exception &error) {
//        qDebug() << "Caught exception: " << error.what();
//        return NULL;
//    }
//    /* This returns data in 16bit grayscale format! */
//    /* return (unsigned char*)this->rawBlob->data(); */

//    int w = this->cols;
//    int h = this->rows;
//    unsigned char *myPixels = new unsigned char[3*w*h];
//        Magick::PixelPacket *pp = image.getPixels(0, 0, w, h);
//        for (int i = 0; i < w*h; i++) {
//            myPixels[3*(i+1) - 3] = pp[i].red;
//            myPixels[3*(i+1) - 2] = pp[i].green;
//            myPixels[3*(i+1) - 1] = pp[i].blue;
//        }

//        return myPixels;
    return NULL;
}

ExamDetails DicomFile::getExamDetails(void)
{
    struct {
        DcmTagKey key;
        const char *desc;
    } details[] = {
    { DCM_PatientName,      "patientName"      },
    { DCM_PatientID,        "patientID"        },
    { DCM_PatientAge,       "patientAge"       },
    { DCM_PatientSex,       "patientSex"       },
    { DCM_PatientBirthDate, "patientBirthDate" },
    { DCM_StudyDate,        "studyDate"	       },
    { DCM_StudyTime,        "studyTime"        },
    { DCM_InstitutionName,  "institutionName"  },
    /* CT specific tags */
    { DCM_KVP,              "kvp"              },
    { DCM_XRayTubeCurrent,  "xrayTubeCurrent"  },
    { DCM_Exposure,         "exposure"         },
    { DCM_GantryDetectorTilt, "gantryDetectorTilt" },
    { DCM_SliceThickness,   "sliceThickness"   }
};

    /* Calculate size of array */
    size_t len = sizeof(details) / sizeof(details[0]);

    /* Retrieve values for keys and populate the map */
    for (unsigned int i = 0; i < len; i++) {
        QString result = DicomHelper::getDcmTagKeyAsQString(this->pDcmDataset, details[i].key);
        this->examDetails.insert(details[i].desc, result);
    }

    /* Get the modality of the exam (CT, MRI, etc) */
    OFString result;
    OFCondition status = this->pDcmDataset->findAndGetOFString(DCM_Modality, result);
    if (status.good()) {
        if (result == "CT") {
        } else if (result == "MRI") {
        } else {
            qDebug() << "Unknown imaging modality. We will not show examination " \
                        "specific details";
        }
    }

    return ExamDetails(this->examDetails);
}

/*
 * Return the maximum CT value for all pixels of the image in the current
 * DICOM file. We need to find the maximum CT value among all DICOM files,
 * so that we normalize correctly. Otherwise, if we do it on a per DICOM
 * file basis, the images will have different colors and flickering will happen.
 */
float DicomFile::getMaxPixel(void) const
{
    return this->maxPixel;
}
