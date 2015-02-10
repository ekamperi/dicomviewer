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

signals:
    void openDICOM_files(void);
    void openDICOM_dir(void);

private slots:
    void on_btnOpenDICOMFiles_clicked();

    void on_btnOpenDICOMDir_clicked();

    void on_btnExit_clicked();

private:
    Ui::StartupMenu *ui;
};

#endif // STARTUPMENU_H
