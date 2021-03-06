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

    void scanPath(QString path);

protected:
    virtual void keyPressEvent(QKeyEvent *event);

signals:
    void loadSeries(const QList<QString> &);
    void hideMyDockWidget(void);

public slots:
    void updateUI(bool isFloating);

private slots:
    void on_btnBrowse_clicked();
    void on_itemSelectionChanged(void);
    void on_itemDoubleClicked(QTreeWidgetItem *, int);
    void on_editPath_returnPressed();
    void loadDroppedPatient(void);

    /* This is triggered when the FindDicomWorker is finished */
    void filesScanned();
    void readProgress(unsigned int scannedFiles, unsigned int parsedFiles);
    void progressDialogCanceled();

    void on_btnScan_clicked();

private:
    void doScan(QString dir, bool isRecursive);
    QTreeWidgetItem *addTreeRoot(const void *obj, int type);
    QTreeWidgetItem *addTreeChild(QTreeWidgetItem *parent, const void *obj, int type);
    PatientExplorer *pPatientExplorer;
    QProgressDialog *progressDialog;

    Ui::PatientExplorerWidget *ui;
};

#endif // PATIENTEXPLORERWIDGET_H
