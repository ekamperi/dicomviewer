#ifndef PATIENTEXPLORERWIDGET_H
#define PATIENTEXPLORERWIDGET_H

#include <QWidget>
#include <QProgressDialog>
#include <QTreeWidgetItem>

#include "patientexplorer.h"

namespace Ui {
class PatientExplorerWidget;
}

// Qt documentation states that user types should begin at this value.
// Values smaller than these are reserved by Qt.
static const int TypePatient = QTreeWidgetItem::UserType;
static const int TypeStudy   = QTreeWidgetItem::UserType + 1;
static const int TypeSeries  = QTreeWidgetItem::UserType + 2;

class PatientExplorerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PatientExplorerWidget(QWidget *parent = 0);
    ~PatientExplorerWidget();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private slots:
    void on_btnBrowse_clicked();
    void on_itemSelectionChanged(void);

    /* This is triggered when then FindDicomWorker is finished */
    void filesScanned();
    void readProgress(unsigned int scannedFiles, unsigned int parsedFiles);
    void progressDialogCanceled();


    void on_editPath_returnPressed();

private:
    void doScan(QString dir);
    QTreeWidgetItem *addTreeRoot(const void *obj, int type);
    QTreeWidgetItem *addTreeChild(QTreeWidgetItem *parent, const void *obj, int type);
    PatientExplorer *pPatientExplorer;
    QProgressDialog *progressDialog;

    Ui::PatientExplorerWidget *ui;
};

#endif // PATIENTEXPLORERWIDGET_H
