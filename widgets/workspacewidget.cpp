#include "include/widgets/workspacewidget.h"
#include "include/widgets/gridwidget.h"

#include <QDebug>
#include <QTreeWidget>

WorkspaceWidget::WorkspaceWidget(QWidget *parent) :
    QWidget(parent)
{
    this->pHLayout = new QHBoxLayout;
    Q_ASSERT(this->pHLayout);
}

void WorkspaceWidget::loadPatient(QTreeWidget *pTreeWidget)
{
    qDebug() << Q_FUNC_INFO;

    /* Create a grid widget that will show the thumbnails */
    GridWidget *pGridWidget = new GridWidget;
    Q_ASSERT(pGridWidget);

    /* Set the stretch factors */
    this->pHLayout->setStretchFactor(pGridWidget, 50);
    this->pHLayout->setStretchFactor(pTreeWidget, 15);
    pTreeWidget->setMaximumWidth(250);

    /* Populate the layout with widgets */
    this->pHLayout->addWidget(pTreeWidget);
    this->pHLayout->addWidget(pGridWidget);

    /* Ready! */
    this->setLayout(pHLayout);

}
