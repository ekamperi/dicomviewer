#include "mymath.h"
#include "topogram.h"

#include <QtGlobal>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>

Topogram::Topogram(QWidget *parent) : QWidget(parent)
{
    qDebug() << Q_FUNC_INFO;
}

Topogram::Topogram(float *pRawData, int width, int height, int sliceIndex,
                   QWidget *parent) : QWidget(parent)
{
    qDebug() << Q_FUNC_INFO;

    this->pRawData = pRawData;
    this->rawHeight = height;
    this->rawWidth = width;
    this->sliceIndex = sliceIndex;
    this->totalSlices = height; // XXX

    /* Convert floating point pixel data [0.0, 1.0] to unsigned char [0, 255]
     * QImage cannot handle floating point data!
     */
    this->pConvertedData =
            MyMath::floatToByte(
                this->pRawData, this->rawWidth, this->rawHeight);

    /* Quoting from the Qt docs:
     * The buffer must remain valid throughout the life of the QImage.
     * The image does NOT delete the buffer at destruction.
     */
    this->pImage = new QImage(this->pConvertedData,
                              this->rawWidth, this->rawHeight,
                              QImage::Format_Indexed8);
    Q_ASSERT(this->pImage);

    QVector<QRgb> my_table;
    for (int i = 0; i < 256; i++) my_table.push_back(qRgb(i,i,i));
    this->pImage->setColorTable(my_table);

    /* XXX: Take slice thickness into consideration for height */
    this->setGeometry(0, 0, width, 5*height);
 }

Topogram::~Topogram()
{
    delete this->pImage;
    delete this->pConvertedData;
}

void Topogram::mouseMoveEvent(QMouseEvent *pEvent)
{
    if (pEvent->buttons() & Qt::RightButton) {
        float newTmin;
        float newTmax;
        float x = pEvent->localPos().x();
        float y = pEvent->localPos().y();

        float width = y / ((float) this->height());
        float center = x / ((float) this->width());

        newTmin = center - width/2;
        newTmax = center + width/2;

        if (newTmin < 0.0) newTmin = 0.0;
        if (newTmax > 1.0) newTmax = 1.0;

        if (qAbs(newTmin - this->tmin) > 0.005) this->tmin = newTmin;
        if (qAbs(newTmax - this->tmax) > 0.005) this->tmax = newTmax;

        if (this->tmin == newTmin || this->tmax == newTmax) {
            delete this->pImage;
            delete this->pConvertedData;
            this->pConvertedData =
                    MyMath::floatToByte(
                        this->pRawData, this->rawWidth, this->rawHeight,
                        this->tmin, this->tmax);

            QImage *pNewImage = new QImage(
                        this->pConvertedData,
                        this->rawWidth, this->rawHeight, QImage::Format_Indexed8);
            Q_ASSERT(pNewImage);
            this->pImage = pNewImage;
            this->update();
        }
    }
}

void Topogram::paintEvent(QPaintEvent *pEvent)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << this->sliceIndex << this->totalSlices;

    /* Create a scaled version of the topogram, based on the new size */
    QImage scaledImage =
            this->pImage->scaled(
                size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    /* Do the actual drawing */
    QPainter painter;
    painter.begin(this);
        painter.drawPixmap(0, 0, width(), height(),
                           QPixmap::fromImage((scaledImage)));
        /* Calculate the new position of the red line */
        float ypos = ((float)(this->sliceIndex) / this->totalSlices) * height();
        QPen pen(Qt::red, 2);
        painter.setPen(pen);
        painter.drawLine(0, ypos, width(), ypos);
    painter.end();
}

void Topogram::setNewSliceIndex(unsigned int newSliceIndex)
{
    this->sliceIndex = newSliceIndex;
    this->update();
}
