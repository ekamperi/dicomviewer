#include "mainwindow.h"
#include "include/widgets/startupmenu.h"
#include "ui_startupmenu.h"

StartupMenu::StartupMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartupMenu)
{
    ui->setupUi(this);
    this->setAcceptDrops(true);
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

void StartupMenu::dropEvent(QDropEvent *event)
{
    qDebug() << Q_FUNC_INFO;

    event->acceptProposedAction();
    emit this->patientDropped();
}

void StartupMenu::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    event->acceptProposedAction();
}
