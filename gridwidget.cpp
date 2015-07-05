#include "gridwidget.h"
#include "myimagewidget.h"

#include <QDebug>

GridWidget::GridWidget(QWidget *parent) :
    QScrollArea(parent)
{
    this->pContainerWidget = new QWidget;
    this->flowLayout = new FlowLayout;
}

void GridWidget::addSlices(const QVector<Slice *> &vecSlices)
{
    qDebug() << Q_FUNC_INFO;

    /* Populate the flow grid layout */
    int howMany = vecSlices.size();
    for (int i = 0; i < howMany; i++) {
        Slice *pSlice = vecSlices.at(i);
        Q_ASSERT(pSlice);

        MyImageWidget *pMyImageWidget = new MyImageWidget();
        Q_ASSERT(pMyImageWidget);

        pSlice->setImageWidget(pMyImageWidget);
        pMyImageWidget->setSlice(pSlice);

        connect(pMyImageWidget, SIGNAL(sliceDoubleClicked(Slice*)),
                this, SLOT(sliceDoubleClicked(Slice*)));

        this->flowLayout->addWidget(pMyImageWidget);
    }

    this->pContainerWidget->setLayout(this->flowLayout);
    this->setWidget(this->pContainerWidget);
    this->setBackgroundRole(QPalette::Dark);
    this->setWidgetResizable(true);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void GridWidget::sliceDoubleClicked(Slice *pSlice)
{
    Q_ASSERT(pSlice);
    qDebug() << Q_FUNC_INFO << "sliceIndex =" << pSlice->getIndex();
// XXX    gotoSlice(pSlice->getIndex());
}
