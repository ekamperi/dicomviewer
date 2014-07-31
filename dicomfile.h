#ifndef DICOMVIEWER_H
#define DICOMVIEWER_H

#include "Magick++.h"

#include <QMap>
#include <QString>

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

class DicomFile
{
public:
    explicit DicomFile();
    ~DicomFile();

    void loadDicomFile(QString filename);

    void parseDicomFile(QString filename);
    void getRawPixel(QString filename);

    unsigned char *getCompressedData();
    const QString *getPatient();

    unsigned int getWidth() const { return this->cols; }
    unsigned int getHeight() const { return this->rows; }

private:
    QString filename;
    DcmFileFormat dcmFileFormat;
    DcmDataset *dcmDataset;

    /* Image data */
    Magick::Blob *rawBlob;
    unsigned int rows;
    unsigned int cols;

    QString patientName;

    QList< QMap<QString, QString> > *list;
    QMap<QString, QString> parseDicomFromXml(const char *s);

    unsigned char *jp2k_to_png(Uint8* pixelData, Uint32 length);
};

#endif // DICOMVIEWER_H
