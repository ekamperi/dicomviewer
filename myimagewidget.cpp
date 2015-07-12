#include "myimagewidget.h"
#include "mymath.h"
#include "slice.h"

#include <QtDebug>
#include <QPainter>

MyImageWidget::MyImageWidget(QWidget *parent) : QLabel(parent)
{
    /* This is used to implement a hover like effect */
    this->weAreIn = false;

    /* Set default size of the thumbnail */
    this->setFixedSize(256, 256);

    /* Generate color table */
    for (unsigned int i = 0; i < 256; i++) {
        myColorTable.append(qRgb(i,i,i));
    }

    /* */
    this->pData = NULL;
    this->pImage = NULL;
}

MyImageWidget::~MyImageWidget()
{
    if (this->pImage) {
        delete this->pImage;
    }
    if (this->pData) {
        delete this->pData;
    }
}

void MyImageWidget::setSlice(Slice *pSlice)
{
    Q_ASSERT(pSlice);

    this->pSlice = pSlice;
    this->genImage();
    this->update();
}

void MyImageWidget::genImage()
{
    unsigned int width  = this->pSlice->getWidth();
    unsigned int height = this->pSlice->getHeight();
    float *pRawData = this->pSlice->getRawPixelData();
    Q_ASSERT(pRawData);

    /* Apply window/width and also map [0.0, 1.0] -> [0,255] because QImage
     * cannot handle floating point data.
    */
    if (this->pData) {
        delete this->pData;
    }
    this->pData =
            MyMath::floatToByte(
                pRawData, width, height,
                0.0, 0.4);

    /* Create the image, but first delete the old one, if any */
    if (this->pImage) {
        delete this->pImage;
    }
    this->pImage = new QImage(this->pData, width, height, QImage::Format_Indexed8);
    Q_ASSERT(pImage);

    /* We are done, display the image */
    pImage->setColorTable(myColorTable);
    this->setPixmap(QPixmap::fromImage((this->pImage->scaled(256,256))));
}

void MyImageWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    emit sliceDoubleClicked((const Slice *)this->pSlice);
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

