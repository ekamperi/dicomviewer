#include <sstream>
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

#include "Magick++.h"

#include "dicomfile.h"

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcpxitem.h"

DicomFile::DicomFile()
{
    /* Allocate memory for list */
    this->list = new QList< QMap<QString, QString> >();
}

DicomFile::~DicomFile()
{
    delete list;
}

void DicomFile::loadDicomFile(QString filename)
{
    qDebug() << Q_FUNC_INFO;

    this->filename = filename;
    OFCondition status = this->dcmFileFormat.loadFile(filename.toStdString().c_str());
    if (!status.good()) {
        qDebug() << status.text();
    }
    this->dcmDataset = this->dcmFileFormat.getDataset();
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
        this->list->append(parseDicomFromXml(ss.str().c_str()));
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

unsigned char *DicomFile::getCompressedData()
{
    qDebug() << Q_FUNC_INFO;

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
        qDebug() << "About to call jp2k_to_png()";
        return jp2k_to_png(pixData, length);
    }
    qDebug() << status.text();
    return NULL;
}

unsigned char *DicomFile::jp2k_to_png(Uint8* pixelData, Uint32 length)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "pixelData = " << pixelData << "length = " << length;

    Q_ASSERT(pixelData != NULL);
    Q_ASSERT(length > 0);

    this->rawBlob = new Magick::Blob(pixelData, sizeof(Uint8) * length);
    Magick::Image image;
    try {
        image.magick("RGB");
        image.read(*this->rawBlob);

        this->rows = image.rows();
        this->cols = image.columns();
        qDebug() << "columns =" << image.columns()
                 << "rows =" << image.rows();
    } catch (Magick::Exception &error) {
        qDebug() << "Caught exception: " << error.what();
        return NULL;
    }
    /* This returns data in 16bit grayscale format! */
    /* return (unsigned char*)this->rawBlob->data(); */

    int w = this->cols;
    int h = this->rows;
    unsigned char *myPixels = new unsigned char[3*w*h];
        Magick::PixelPacket *pp = image.getPixels(0, 0, w, h);
        for (int i = 0; i < w*h; i++) {
            myPixels[3*(i+1) - 3] = pp[i].red;
            myPixels[3*(i+1) - 2] = pp[i].green;
            myPixels[3*(i+1) - 1] = pp[i].blue;
        }

        return myPixels;
}

QString DicomFile::getDcmTagKeyAsQString(const DcmTagKey &dcmTagKey)
{
    qDebug() << Q_FUNC_INFO;

    /* Retrieve name */
    OFString result;
    OFCondition status = this->dcmDataset->findAndGetOFString(
                dcmTagKey, result);
    if (status.good()) {
        return QString(result.c_str());
    } else {
        qDebug() << status.text();
        return QString("NOTAVAIL");
    }
}

ExamDetails DicomFile::getExamDetails(void)
{
    qDebug() << Q_FUNC_INFO;

    struct {
        DcmTagKey key;
        const char *desc;
    } details[] = {
    { DCM_PatientName,      "patientName"      },
    { DCM_PatientID,        "patientID"        },
    { DCM_PatientAge,       "patientAge"       },
    { DCM_PatientSex,       "patientSex"       },
    { DCM_PatientBirthDate, "patientBirthDate" },
};

    /* Calculate size of array */
    size_t len = sizeof(details) / sizeof(details[0]);

    /* Retrieve values for keys and populate the map */
    for (unsigned int i = 0; i < len; i++) {
        QString result = getDcmTagKeyAsQString(details[i].key);
        this->examDetails.insert(details[i].desc, result);

        qDebug() << details[i].desc << " =" << result;
    }

    return ExamDetails(this->examDetails);
}
