#ifndef DICOMVIEWER_H
#define DICOMVIEWER_H

#include "examdetails.h"
#include "hounsfieldunit.h"
//#include "Magick++.h"
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
    float getMaxPixel(void) const;

    float getHorizontalSpacing(void) const { return this->hSpacing; }
    float getVerticalSpacing(void) const { return this->vSpacing; }

    HounsFieldUnit getDefaultHUF(void) const { return this->defHUF; }

private:
    QString filename;
    DcmFileFormat dcmFileFormat;
    DcmDataset *pDcmDataset;

    float vSpacing;
    float hSpacing;

    Uint16 maxPixel;
    HounsFieldUnit defHUF;

    /* Image data */
    //Magick::Blob *pRawBlob;
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
