#ifndef PATIENTEXPLORERWIDGET_H
#define PATIENTEXPLORERWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>

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

private:
    QTreeWidgetItem *addTreeRoot(QString name);
    void addTreeChild(QTreeWidgetItem *parent, QString name);

    Ui::PatientExplorerWidget *ui;
};

#endif // PATIENTEXPLORERWIDGET_H
