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

        /* If user double clicks on a slice, we'd like to load it.
         * Hence, propagate the signa.  */
        connect(pMyImageWidget, SIGNAL(sliceDoubleClicked(const Slice *)),
                          this, SIGNAL(sliceDoubleClicked(const Slice *)));

        this->flowLayout->addWidget(pMyImageWidget);
    }

    this->pContainerWidget->setLayout(this->flowLayout);
    this->setWidget(this->pContainerWidget);
    //this->setBackgroundRole(QPalette::Dark);
    this->setWidgetResizable(true);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /* Set background color */
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor("#CFCFCF"));  // XXX: find best color
    this->setAutoFillBackground(true);
    this->setPalette(pal);
}

