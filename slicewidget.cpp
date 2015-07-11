#include "slicewidget.h"
#include "myimagewidget.h"

SliceWidget::SliceWidget(QVector<Slice *> *pVecSlices, QWidget *parent) : QWidget(parent)
{
    /* This is where our slices will be saved */
    Q_ASSERT(pVecSlices);
    this->vecSlices = pVecSlices;

    /* The GL Widget is responsible for drawing the graphics */
    this->pGLWidget = new MyGLWidget();
    Q_ASSERT(this->pGLWidget);

    this->pHLayout = new QHBoxLayout();
    Q_ASSERT(this->pHLayout);

    this->pHLayout->addWidget(this->pGLWidget);
    this->setLayout(this->pHLayout);

    /* Connect the signals */
    connect(this->pGLWidget, SIGNAL(sliceChanged(int)),
            this, SLOT(gotoSlice(int)));

    /* We also need a vertical scrollbar to navigate through slices */
    this->pScrollBar = new QScrollBar;
    Q_ASSERT(this->pScrollBar);
    this->pScrollBar->setMinimum(0);    // XXX
    connect(this->pScrollBar, SIGNAL(valueChanged(int)),
            this, SLOT(scrollBarValueChanged(int)));
    this->pHLayout->addWidget(this->pScrollBar);
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
            emit this->backToGridWidget();
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
    // XXX: Hack
    this->pScrollBar->setMaximum(this->vecSlices->size()-1);

    /* Check whether we are inside the bounds or we are recycling */
    if (idx < 0) {
        idx = vecSlices->size() - 1;
    } else if (idx > vecSlices->size()-1) {
        idx = 0;
    }

    /* Set new slice */
    this->pGLWidget->setSlice(this->vecSlices->at(idx));

    /* Update scrollbar value */
    this->pScrollBar->setValue(idx);

    /*
     * Notify others that the current slice has changed (e.g. main window
     * and the global status bar.
    */
    emit this->sliceChanged(idx);
}

void SliceWidget::gotoSlice(const Slice *pSlice)
{
    Q_ASSERT(pSlice);
    gotoSlice(pSlice->getIndex());
}

void SliceWidget::scrollBarValueChanged(int value)
{
    qDebug() << Q_FUNC_INFO << value;
    this->gotoSlice(value);
}

