#include <sstream>
#include <QDebug>
#include <QFile>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QTemporaryFile>
#include <QXmlStreamReader>

#include "Magick++.h"

#include "dicomfile.h"
#include "ui_dicomfile.h"

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcpxitem.h"

DicomFile::DicomFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DicomFile)
{
    ui->setupUi(this);

    /* XXX: Temporary hack until we find out how to make the
     * table widget to expand into full form width.
     */
    int x0 = ui->tableWidget->pos().rx();
    int y0 = ui->tableWidget->pos().ry();
    ui->tableWidget->setMinimumSize(this->width()-x0, this->height()-y0);

    /* Allocate memory for list */
    this->list = new QList< QMap<QString, QString> >();
}

DicomFile::~DicomFile()
{
    delete ui;
    delete list;
}

void DicomFile::setFileName(QString filename)
{
    this->filename = filename;
}

void DicomFile::loadDicomFile(QString filename)
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

    /* Populate table */
    ui->tableWidget->setRowCount(2*pDcmDataset->card());
    ui->tableWidget->setColumnCount(5);

    i = 0;
    while (!this->list->isEmpty()) {
        QMap<QString, QString> e = this->list->takeFirst();
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(e["name"]));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(e["tag"]));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(e["len"]));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(e["vr"]));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(e["text"]));
        i++;
    }

    /* Set header labels
     * This must follow the table population */
    ui->tableWidget->setHorizontalHeaderLabels(
                QStringList() << "Name" << "Tag" << "Len" << "VR" << "Value");
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
    OFCondition cond = EC_Normal;
    DcmElement* element = NULL;

    cond = pDcmDataset->findAndGetElement(DCM_PixelData, element);
    if (cond.bad() || element == NULL)
        return NULL;

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
    cond = dpix->getEncapsulatedRepresentation(xferSyntax, rep, dseq);
    if (cond == EC_Normal) {
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
        if (length == 0) return NULL;

        /* Finally, get the compressed data for this pixel item */
        Uint8 *pixData = NULL;
        cond = pixitem->getUint8Array(pixData);

        /* Convert jpeg2000 to png */
        qDebug() << "About to call jp2k_to_png()";
        return jp2k_to_png(pixData, length);
    }
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
