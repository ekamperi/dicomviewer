#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dicomviewer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
}

void MainWindow::on_actionOpenDICOM_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this);

    DicomViewer *pDicomViewer = new DicomViewer;
    pDicomViewer->loadDicomFile(filename);
    pDicomViewer->setAttribute(Qt::WA_DeleteOnClose);
    pDicomViewer->show();
}
