#include <sstream>
#include <QDebug>
#include <QFile>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QTemporaryFile>
#include <QXmlStreamReader>

#include "Magick++.h"

#include "dicomviewer.h"
#include "ui_dicomviewer.h"

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcpxitem.h"

DicomViewer::DicomViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DicomViewer)
{
    ui->setupUi(this);

    /* XXX: Temporary hack until we find out how to make the
     * table widget to expand into full form width.
     */
    int x0 = ui->tableWidget->pos().rx();
    int y0 = ui->tableWidget->pos().ry();
    ui->tableWidget->setMinimumSize(
                this->width()-x0, this->height()-y0);

    /* Allocate memory for list */
    this->list = new QList< QMap<QString, QString> >();
}

DicomViewer::~DicomViewer()
{
    delete ui;
    delete list;
}

void DicomViewer::loadDicomFile(QString filename)
{
    DcmFileFormat dcmFile;
    OFCondition status = dcmFile.loadFile(filename.toStdString().c_str());
    if (status.good()) {
        OFString patientName;
        status = dcmFile.getDataset()->findAndGetOFString(
                    DCM_PatientName, patientName);
        if (status.good()) {
            qDebug() << "Patient name:" << patientName.c_str();
        } else {
            qWarning() << status.text();
        }
    } else {
        qWarning() << status.text();
    }

    /* Iterate over all objects */
    DcmDataset *pDcmDataset = dcmFile.getDataset();
    DcmStack stack;
    DcmObject *obj = NULL;

    getCompressedData(pDcmDataset);

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

QMap<QString, QString> DicomViewer::parseDicomFromXml(const char *s)
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

void DicomViewer::getCompressedData(DcmDataset *pDcmDataset)
{
    OFCondition cond = EC_Normal;
    DcmElement* element = NULL;

    cond = pDcmDataset->findAndGetElement(DCM_PixelData, element);
    if (cond.bad() || element == NULL)
        return;

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
			return;

        /*
         * Get the length of this pixel item (i.e. fragment, i.e. most of
         * the time, the length of the frame).
         */
		Uint32 length = pixitem->getLength();
        if (length == 0) return;

        /* Finally, get the compressed data for this pixel item */
        Uint8 *pixData = NULL;
        cond = pixitem->getUint8Array(pixData);

        /* Convert jpeg2000 to png */
        jp2k_to_png(pixData, length);
    }
}

void DicomViewer::jp2k_to_png(Uint8* pixelData, Uint32 length)
{
    /* Write data to a temporary file */
    QTemporaryFile tmpFile;

    if (tmpFile.open())  {
        tmpFile.write((char *)pixelData, length * sizeof(Uint8));
    } else {
        qDebug() << "Error creating temporary file";
        return;
    }

    Magick::Blob blob(pixelData, length);
    Magick::Image image(blob);

    image.magick("PNG");
    image.write(&blob);

    Magick::Image image2(blob);
    image2.write("/home/stathis/kaka.png");
}
