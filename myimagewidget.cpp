#include "myimagewidget.h"
#include "slice.h"

#include <QtDebug>
#include <QPainter>

MyImageWidget::MyImageWidget(QWidget *parent) : QLabel(parent)
{
    qDebug() << Q_FUNC_INFO;

    /* This is used to implement a hover like effect */
    this->weAreIn = false;

    this->setFixedSize(256, 256);

    /* Generate color table */
    for (unsigned int i = 0; i < 256; i++) {
        myColorTable.push_back(qRgb(i,i,i));
    }
}

MyImageWidget::~MyImageWidget()
{
    qDebug() << Q_FUNC_INFO;
}

void MyImageWidget::setSlice(Slice *pSlice)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(pSlice);

    this->pSlice = pSlice;
    this->loadTexture(
                pSlice->getRawPixelData(),
                pSlice->getWidth(),
                pSlice->getHeight(),
                pSlice->getFormat());
    this->update();
}

void MyImageWidget::loadTexture(Uint8* pRawPixel,
                             unsigned int width,
                             unsigned int height,
                             GLint format)
{
    qDebug() << Q_FUNC_INFO;

    this->pImage = new QImage((Uint8 *)pRawPixel, width, height, QImage::Format_Indexed8);
    Q_ASSERT(pImage);
    pImage->setColorTable(myColorTable);
    this->setPixmap(QPixmap::fromImage((this->pImage->scaled(256,256))));
}

void MyImageWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    emit sliceDoubleClicked(this->pSlice);
}

void MyImageWidget::drawOutline(QPainter *pPainter)
{
    Q_ASSERT(pPainter);

    if (this->weAreIn || this->pSlice->isSelected()) {
        QPen oldPen, myPen;
        oldPen = pPainter->pen();
        myPen.setWidth(4);
        myPen.setColor(Qt::red);
        pPainter->setPen(myPen);
        pPainter->drawRect(QRect(0, 0, this->width(), this->height()));
        pPainter->setPen(oldPen);
    }
}

void MyImageWidget::enterEvent(QEvent * event)
{
    this->weAreIn = true;
    this->update();

    QWidget::enterEvent(event);
}

void MyImageWidget::leaveEvent(QEvent * event)
{
    this->weAreIn = false;
    this->update();

    QWidget::leaveEvent(event);
}

void MyImageWidget::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPainter painter;
    painter.begin(this);
    this->drawOutline(&painter);
    painter.end();
}
