#include "mainwindow.h"
#include "startupmenu.h"
#include "ui_startupmenu.h"

StartupMenu::StartupMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartupMenu)
{
    ui->setupUi(this);
}

StartupMenu::~StartupMenu()
{
    delete ui;
}

void StartupMenu::on_btnOpenDICOMFiles_clicked()
{
    emit openDICOM_files();
}

void StartupMenu::on_btnOpenDICOMDir_clicked()
{
    emit openDICOM_dir();
}

void StartupMenu::on_btnOpenPatientExplorer_clicked()
{
    emit openPatientExplorer();
}

void StartupMenu::on_btnExit_clicked()
{
    QCoreApplication::exit(0);
}
