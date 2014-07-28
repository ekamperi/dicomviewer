#ifndef DICOMVIEWER_H
#define DICOMVIEWER_H

#include <QMap>
#include <QString>
#include <QWidget>

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
};

#endif // DICOMVIEWER_H
