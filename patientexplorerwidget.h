#ifndef PATIENTEXPLORERWIDGET_H
#define PATIENTEXPLORERWIDGET_H

#include <QWidget>
#include <QProgressDialog>
#include <QTreeWidgetItem>

#include "patientexplorer.h"

namespace Ui {
class PatientExplorerWidget;
}

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
    void readProgress(unsigned int scannedFiles);
    void progressDialogCanceled();


private:
    QTreeWidgetItem *addTreeRoot(QString name, QString desc);
    QTreeWidgetItem *addTreeChild(QTreeWidgetItem *parent, const Study &study);
    QTreeWidgetItem *addTreeSeries(QTreeWidgetItem *parent, const Series &series);

    PatientExplorer *pPatientExplorer;
    QProgressDialog *progressDialog;

    Ui::PatientExplorerWidget *ui;
};

#endif // PATIENTEXPLORERWIDGET_H
