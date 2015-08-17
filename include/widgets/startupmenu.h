#ifndef STARTUPMENU_H
#define STARTUPMENU_H

#include <QWidget>

namespace Ui {
class StartupMenu;
}

class StartupMenu : public QWidget
{
    Q_OBJECT

public:
    explicit StartupMenu(QWidget *parent = 0);
    ~StartupMenu();

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

signals:
    void openDICOM_files(void);
    void openDICOM_dir(void);
    void openPatientExplorer(void);
    void patientDropped(void);

private slots:
    void on_btnOpenDICOMFiles_clicked();
    void on_btnOpenDICOMDir_clicked();
    void on_btnOpenPatientExplorer_clicked();
    void on_btnExit_clicked();

private:
    Ui::StartupMenu *ui;
};

#endif // STARTUPMENU_H
