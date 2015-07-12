#include "gridwidget.h"
#include "myimagewidget.h"

#include <QApplication>
#include <QDebug>
#include <QKeyEvent>

GridWidget::GridWidget(QWidget *parent) :
    QWidget(parent)
{    
    this->pScrollArea = new QScrollArea;
    this->pContainerWidget = new QWidget;
    this->pFlowLayout = new FlowLayout(this);
    this->pVLayout = new QVBoxLayout;
}

void GridWidget::addSlices(const QVector<Slice *> &vecSlices)
{
    qDebug() << Q_FUNC_INFO;

    this->pVecSlices = &vecSlices;

    /* Populate the flow grid layout */
    int howMany = vecSlices.size();
    for (int i = 0; i < howMany; i++) {
        Slice *pSlice = vecSlices.at(i);
        Q_ASSERT(pSlice);

        MyImageWidget *pMyImageWidget = new MyImageWidget();
        Q_ASSERT(pMyImageWidget);

        /* Associate slice <---> image widget */
        pSlice->setImageWidget(pMyImageWidget);
        pMyImageWidget->setSlice(pSlice);

        /* */
        connect(pSlice, SIGNAL(iNeedRepaint(float,float)),
                pMyImageWidget, SLOT(changeWindow(float, float)));

        /* If user double clicks on a slice, we'd like to load it.
         * Hence, propagate the signa.  */
        connect(pMyImageWidget, SIGNAL(sliceDoubleClicked(const Slice *)),
                          this, SIGNAL(sliceDoubleClicked(const Slice *)));

        this->pFlowLayout->addWidget(pMyImageWidget);
    }

    this->pContainerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->pContainerWidget->setLayout(this->pFlowLayout);
    this->pScrollArea->setWidgetResizable(true);
    this->pScrollArea->setWidget(this->pContainerWidget);
    this->setLayout(this->pVLayout);
    this->pVLayout->addWidget(this->pScrollArea);

    /* Set background color */
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor("#D3D3D3"));
    this->pContainerWidget->setAutoFillBackground(true);
    this->pContainerWidget->setPalette(pal);
}

void GridWidget::selectAllSlices(void)
{
    qDebug() << Q_FUNC_INFO;

    int howMany = this->pVecSlices->size();
    for (int i = 0; i < howMany; i++) {
        Slice *pSlice = this->pVecSlices->at(i);
        Q_ASSERT(pSlice);
        bool isSelected = pSlice->isSelected();
        pSlice->setSelected(!isSelected);
        pSlice->getImageWidget()->update();
    }
}

void GridWidget::changeWindow(HUWindows::window newWindow)
{
    qDebug() << Q_FUNC_INFO;

    emit this->heavyTaskInitiated();

    int nSlices = this->pVecSlices->size();
    for (int i = 0; i < nSlices; i++) {
        Slice *pSlice = this->pVecSlices->at(i);
        Q_ASSERT(pSlice);

        /* This will trigger an 'iNeedRepaint' signal and all connected
         * widgets will get notified (myimagewidget, myglwidget, etc). */
        pSlice->setWindow(newWindow);
    }

    emit this->heavyTaskCompleted();
}

bool GridWidget::event(QEvent *pEvent)
{
    if (pEvent->type() == QEvent::KeyPress) {
        QKeyEvent *pke = static_cast<QKeyEvent *>(pEvent);
        int key = pke->key();
        if (key == Qt::Key_Escape) {
            //this->on_actionClose_triggered();
        } else if (key == Qt::Key_A
                   && (QApplication::keyboardModifiers() & Qt::ControlModifier)) {
            selectAllSlices();
        }
    }
    return true;
}
