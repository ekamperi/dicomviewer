#include "slicewidget.h"
#include "myimagewidget.h"

SliceWidget::SliceWidget(QVector<Slice *> *pVecSlices, QWidget *parent) : QWidget(parent)
{
    /* This is where our slices will be saved */
    Q_ASSERT(pVecSlices);
    this->vecSlices = pVecSlices;

    /* Create a container widget that will hold all other stuff */
    this->pContainerWidget = new QWidget;
    Q_ASSERT(this->pContainerWidget);

    /* The GL Widget is responsible for drawing the graphics */
    this->pGLWidget = new MyGLWidget();
    Q_ASSERT(this->pGLWidget);

    QVBoxLayout *pVLayout = new QVBoxLayout();
    Q_ASSERT(pVLayout);

    pVLayout->addWidget(this->pGLWidget);
    this->setLayout(pVLayout);

    /* Connect the signals */
    connect(this->pGLWidget, SIGNAL(sliceChanged(int)),
            this, SLOT(gotoSlice(int)));
}

SliceWidget::~SliceWidget()
{
    Q_ASSERT(this->pGLWidget);
    delete this->pGLWidget;
    this->pGLWidget = NULL;
}

void SliceWidget::gotoNextSlice(void)
{
    this->gotoSlice(SliceDirection::Next);
}

void SliceWidget::gotoPrevSlice(void)
{
    this->gotoSlice(SliceDirection::Prev);
}

void SliceWidget::gotoSlice(SliceDirection::is dir)
{
    Q_ASSERT(dir == SliceDirection::Prev ||
             dir == SliceDirection::Next);

    unsigned int idx = this->pGLWidget->getSliceIndex();

    if (dir == SliceDirection::Next) {
        gotoSlice(idx + 1);
    } else {
        gotoSlice(idx - 1);
    }
}

bool SliceWidget::event(QEvent *pEvent)
{
    if (pEvent->type() == QEvent::KeyPress) {
        QKeyEvent *pke = static_cast<QKeyEvent *>(pEvent);
        int key = pke->key();
        if (key == Qt::Key_Escape) {
            //this->on_actionClose_triggered();
            return true;
        } else if (key == Qt::Key_PageUp) {
            gotoPrevSlice();
            return true;
        } else if (key == Qt::Key_PageDown || key == Qt::Key_Space) {
            gotoNextSlice();
            return true;
        } else if (key == Qt::Key_Home) {
            gotoSlice(0);
        } else if (key == Qt::Key_End) {
            gotoSlice(vecSlices->size() - 1);
        }
    }

    return QWidget::event(pEvent);
}

void SliceWidget::wheelEvent(QWheelEvent *pEvent)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        pEvent->ignore();
    } else {
        pEvent->accept();
        int delta = pEvent->delta();
        if (delta > 0) {
            this->gotoPrevSlice();
        } else {
            this->gotoNextSlice();
        }
    }
}

void SliceWidget::gotoSlice(int idx)
{
    /* Check whether we are inside the bounds or we are recycling */
    if (idx < 0) {
        idx = vecSlices->size() - 1;
    } else if (idx > vecSlices->size()-1) {
        idx = 0;
    }

    /* Set new slice */
    //ui->stackedWidget->setCurrentWidget(this->pSliceWidget);
    this->pGLWidget->setSlice(this->vecSlices->at(idx));

    /* Update the status bar accordingly */
    //updateStatusBarForSlice();
}

void SliceWidget::gotoSlice(const Slice *pSlice)
{
    Q_ASSERT(pSlice);
    gotoSlice(pSlice->getIndex());
}
