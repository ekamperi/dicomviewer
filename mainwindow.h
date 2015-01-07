#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QProgressDialog>

#include "flowlayout.h"
#include "loaddicomworker.h"
#include "myglwidget.h"

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
    void sliceDoubleClicked(Slice *pSlice);

    void getProgress(unsigned int cnt);
    void progressDialogCanceled();
    void filesLoaded();

    void on_actionExit_triggered();

    void on_actionClose_triggered();

private:
    Ui::MainWindow *ui;

    /* Slices are arranged into a grid layout */
    FlowLayout *flowLayout;

    /* This container widget contains the grid layout and is in turn put
     * into the scroll area. QScrollarea needs a QWidget.
     */
    QWidget *containerWidget;

    QProgressDialog *progressDialog;
    LoadDicomThread *loadDicomThread;
    std::vector<Slice *> slices;
};

#endif // MAINWINDOW_H
