#include "include/widgets/imagewidget.h"
#include "include/helpers/mymath.h"
#include "slice.h"

#include <QtDebug>
#include <QPainter>

ImageWidget::ImageWidget(QWidget *parent) : QLabel(parent)
{
    /* This is used to implement a hover like effect */
    this->weAreIn = false;

    /* Set default size of the thumbnail */
    this->setFixedSize(256, 256);

    /* Generate color table */
    for (unsigned int i = 0; i < 256; i++) {
        myColorTable.append(qRgb(i,i,i));
    }

    // XXX
    this->pData = NULL;
    this->pImage = NULL;
    this->tmin = 0;
    this->tmax = 0;
}

ImageWidget::~ImageWidget()
{
    if (this->pImage) {
        delete this->pImage;
        this->pImage = NULL;
    }
    if (this->pData) {
        delete this->pData;
        this->pData = NULL;
    }
}

void ImageWidget::setSlice(const Slice *pSlice)
{
    Q_ASSERT(pSlice);

    this->pSlice = pSlice;

    /* Get the default window/width level of the slice */
    QPair<float, float> tMinMax = this->pSlice->getDefaultWindowLevelWidth();
    this->tmin = tMinMax.first;
    this->tmax = tMinMax.second;

    this->genImage();
    this->update();
}

void ImageWidget::genImage(void)
{
    Q_ASSERT(this->pSlice);

    /* Get graphics dimensions */
    unsigned int width  = this->pSlice->getWidth();
    unsigned int height = this->pSlice->getHeight();

    /* Get the raw pixel data. Allow null values for formats that we
     * can't understand (e.g. RT doses, etc).
     */
    float *pRawData = this->pSlice->getRawPixelData();
    if (!pRawData) {
        return;
    }

    /* Apply window/width and also map [0.0, 1.0] -> [0,255] because QImage
     * cannot handle floating point data.
    */
    if (this->pData) {
        delete this->pData;
        this->pData = NULL;
    }
    this->pData = MyMath::floatToByte(
                pRawData, width, height, this->tmin, this->tmax);

    /* Create the image, but first delete the old one, if any */
    if (this->pImage) {
        delete this->pImage;
        this->pImage = NULL;
    }
    this->pImage = new QImage(this->pData, width, height, QImage::Format_Indexed8);
    Q_ASSERT(pImage);

    /* We are done, display the actual image */
    pImage->setColorTable(myColorTable);
    this->setPixmap(QPixmap::fromImage((this->pImage->scaled(256, 256))));
}

void ImageWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    qDebug() << Q_FUNC_INFO;
    emit sliceDoubleClicked(this->pSlice);
}

void ImageWidget::drawOutline(QPainter *pPainter)
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

void ImageWidget::enterEvent(QEvent *event)
{
    this->weAreIn = true;
    this->update();

    QWidget::enterEvent(event);
}

void ImageWidget::leaveEvent(QEvent *event)
{
    this->weAreIn = false;
    this->update();

    QWidget::leaveEvent(event);
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPainter painter;
    painter.begin(this);
    this->drawOutline(&painter);
    painter.end();
}

void ImageWidget::changeWindow(HUWindows::window newWindow)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(this->pSlice);

    /* Get the new window/width level of the slice */
    const HUConverter *pHUConverter = this->pSlice->getHUConverter();
    Q_ASSERT(pHUConverter);
    QPair<float, float> tMinMax =
            pHUConverter->getNormalizedRangeFromTemplate(newWindow);

    this->tmin = tMinMax.first;
    this->tmax = tMinMax.second;

    /* Regenerate the image */
    this->genImage();
}
