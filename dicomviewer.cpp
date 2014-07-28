#include <sstream>
#include <QDebug>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QXmlStreamReader>

#include "dicomviewer.h"
#include "ui_dicomviewer.h"

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

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
    /* Allocate memory for map */
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

    int i = 0;
    while (pDcmDataset->nextObject(stack, /* Depth-first */ OFTrue).good()) {
        obj = stack.top();
        std::stringstream ss;
        obj->writeXML(ss);
        qDebug() << ss.str().c_str();
        this->list->append(parseDicomFromXml(ss.str().c_str()));
        i++;
    }

    /* Populate table */
    ui->tableWidget->setRowCount(pDcmDataset->card());
    ui->tableWidget->setColumnCount(5);

    i = 0;
    while(!this->list->isEmpty()) {
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
