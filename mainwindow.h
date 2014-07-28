#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAbout_triggered();

    void on_actionOpenDICOM_triggered();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *pScene;
    QGraphicsView *pView;
};

#endif // MAINWINDOW_H
