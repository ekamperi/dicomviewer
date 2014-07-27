#ifndef DICOMVIEWER_H
#define DICOMVIEWER_H

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
};

#endif // DICOMVIEWER_H
