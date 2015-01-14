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

protected:
    bool event(QEvent *pEvent);
    void wheelEvent(QWheelEvent *event);

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

    /* XXX */
    QScrollArea *scrollArea;

    /* Slices are arranged into a grid layout */
    FlowLayout *flowLayout;

    /* This container widget contains the grid layout and is in turn put
     * into the scroll area. QScrollarea needs a QWidget.
     */
    QWidget *containerWidget;
    QWidget *containerWidget2;

    QProgressDialog *progressDialog;
    LoadDicomThread *loadDicomThread;
    std::vector<Slice *> slices;

    void selectAllSlices(void);

    struct SliceDirection { enum is { Prev, Next }; };
    void gotoSlice(SliceDirection::is dir);
    void gotoNextSlice();
    void gotoPrevSlice();
    void updateStatusBarForSlice(void) const;
};

#endif // MAINWINDOW_H
