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
