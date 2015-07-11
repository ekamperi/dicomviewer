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
#include "slicewidget.h"

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

    void getProgress(unsigned int cnt);
    void progressDialogCanceled();
    void filesLoaded();
    void gotoSlice(const Slice *pSlice);
    void updateStatusBarForSlice(int idx) const;

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

    QVector<Slice *> vecSlices;
    void loadDicomFiles(QStringList fileNames);

    /* Slices are arranged into a grid layout */
    QVBoxLayout *pLayout;

    /* This container widget contains the grid layout and is in turn put
     * into the scroll area. QScrollarea needs a QWidget.
     */
    GridWidget *gridWidget;
    SliceWidget *pSliceWidget;
    StartupMenu *pStartupMenu;

    QProgressDialog *progressDialog;
    LoadDicomThread *loadDicomThread;

    void connectSignals(void) const;
};

#endif // MAINWINDOW_H
