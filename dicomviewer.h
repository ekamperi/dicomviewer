#ifndef DICOMVIEWER_H
#define DICOMVIEWER_H

#include <QMap>
#include <QString>
#include <QWidget>

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

namespace Ui {
class DicomViewer;
}

class DicomViewer : public QWidget
{
    Q_OBJECT

public:
    explicit DicomViewer(QWidget *parent = 0);
    ~DicomViewer();

    void loadDicomFile(QString filename);

private:
    Ui::DicomViewer *ui;
    QList< QMap<QString, QString> > *list;
    QMap<QString, QString> parseDicomFromXml(const char *s);

    void getCompressedData(DcmDataset *);
    void jp2k_to_png(Uint8* pixelData, Uint32 length);
};

#endif // DICOMVIEWER_H
