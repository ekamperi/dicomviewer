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

    /*
     * There are 2 use scenarios here. The Startup menu widget shall respond
     * either of the following:
     *
     * 1. user drags an image series from the patient explorer widget and drops
     * it here, or
     * 2. user drags a folder from his/er desktop and drops it here. In this
     * case we shall call patient explorer widget and let it traverse the file
     * hierarchy just as if the user had clicked 'Browse/Scan'.
     */

    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        QUrl url = event->mimeData()->urls().at(0);
        emit this->directoryDropped(url);
    } else {
        if (event->mimeData()->text() == "start") { // XXX
            event->acceptProposedAction();
            emit this->patientDropped();
        }
    }
}

void StartupMenu::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    event->acceptProposedAction();
}
