#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QProgressDialog>

#include "flowlayout.h"
#include "gridwidget.h"
#include "loaddicomworker.h"
#include "myglwidget.h"
#include "startupmenu.h"

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
    void gotoSlice(const Slice *pSlice);
    void gotoSlice(int sliceIndex);

    void getProgress(unsigned int cnt);
    void progressDialogCanceled();
    void filesLoaded();

    void on_actionExit_triggered();

    void on_actionClose_triggered();

    void on_actionDistance_triggered();

    void on_actionDensity_HUs_triggered();

    void on_actionBone_triggered();

    void on_actionLung_triggered();

    void on_actionMediastinum_triggered();

    void on_actionAbdomen_triggered();

    void on_actionHead_triggered();

    void on_actionFlip_Horizontally_triggered();

    void on_actionFlip_Vertically_triggered();

    void on_actionTopogram_triggered();

    void on_actionOpen_DICOM_dir_triggered();

    void on_actionPan_triggered();

    void on_action_Reset_triggered();

    void on_actionDeleteAllMeasures_triggered();

    void on_actionDeleteSelectedMeasures_triggered();

    void on_actionOpen_patient_explorer_triggered();

private:
    Ui::MainWindow *ui;

    /* Slices are arranged into a grid layout */
    QVBoxLayout *pLayout;

    /* This container widget contains the grid layout and is in turn put
     * into the scroll area. QScrollarea needs a QWidget.
     */
    GridWidget *gridWidget;
    QWidget *containerWidget2;
    StartupMenu *pStartupMenu;

    MyGLWidget *pGLWidget;

    QProgressDialog *progressDialog;
    LoadDicomThread *loadDicomThread;
    QVector<Slice *> vecSlices;

    void loadDCMFiles(QStringList filenames);

    void selectAllSlices(void);

    struct SliceDirection { enum is { Prev, Next }; };
    void gotoSlice(SliceDirection::is dir);
    void gotoNextSlice();
    void gotoPrevSlice();
    void updateStatusBarForSlice(void) const;
};

#endif // MAINWINDOW_H
