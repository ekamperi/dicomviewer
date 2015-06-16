#ifndef PATIENTEXPLORERWIDGET_H
#define PATIENTEXPLORERWIDGET_H

#include <QWidget>
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

private:
    QTreeWidgetItem *addTreeRoot(QString name);
    QTreeWidgetItem *addTreeChild(QTreeWidgetItem *parent, QString name);

    PatientExplorer *pPatientExplorer;

    Ui::PatientExplorerWidget *ui;
};

#endif // PATIENTEXPLORERWIDGET_H
