#ifndef DICOMVIEWER_H
#define DICOMVIEWER_H

#include "examdetails.h"
#include "Magick++.h"
#include "GL/gl.h"

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

    float *getUncompressedData();
    unsigned char *getCompressedData();

    ExamDetails getExamDetails(void);

    unsigned int getWidth() const { return this->cols; }
    unsigned int getHeight() const { return this->rows; }
    GLint getFormat() const { return this->format; }

private:
    QString filename;
    DcmFileFormat dcmFileFormat;
    DcmDataset *pDcmDataset;

    /*
     * Parameters for converting CT data to Hounsfield Units
     * HU = pixelValue * slope + intercept
     */
    float slope;
    float intercept;

    /* Image data */
    Magick::Blob *pRawBlob;
    unsigned int rows;
    unsigned int cols;
    GLint format;

    QString getDcmTagKeyAsQString(const DcmTagKey &dcmTagKey);
    QMap<QString, QString> examDetails;

    QList< QMap<QString, QString> > *pList;
    QMap<QString, QString> parseDicomFromXml(const char *s);

    unsigned char *jp2k_to_png(Uint8* pixelData, Uint32 length);
};

#endif // DICOMVIEWER_H
