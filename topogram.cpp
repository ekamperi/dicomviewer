#include "mymath.h"
#include "topogram.h"

#include <math.h>
#include <QtGlobal>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>

Topogram::Topogram(QWidget *parent) : QWidget(parent)
{
    qDebug() << Q_FUNC_INFO;
}

Topogram::Topogram(QVector<Slice *> vecSlices, float angle,
                   int width, int height, int sliceIndex,
                   QWidget *parent) : QWidget(parent)
{
    qDebug() << Q_FUNC_INFO;

    /* */
    this->genData(vecSlices, angle);

    /* By default the topogram is embedded into the GL Widget */
    this->amIEmbedded = true;

    this->rawHeight = height;
    this->rawWidth = width;
    this->sliceIndex = sliceIndex;
    this->totalSlices = height; // XXX

    this->setStyleSheet("background-color: rgba(0, 0, 0, 50%);");

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
    this->setGeometry(0, 0, width/2, 2.5*height);
 }

Topogram::~Topogram()
{
    delete this->pImage;
    delete this->pConvertedData;
    delete this->pRawData;
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
    } else if (pEvent->buttons() & Qt::LeftButton) {
        int xpos = pEvent->pos().x();
        int ypos = pEvent->pos().y();
        if (ypos < 0 || xpos > this->width()) {
            emit iWantToBreakFree();
        }
    }
}

void Topogram::paintEvent(QPaintEvent *pEvent)
{
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

void Topogram::mouseReleaseEvent(QMouseEvent *pEvent)
{
    /*
     * Change slice if and only if user clicked with *left* button.
     * Because s/he may be releasing the right button while changing
     * window/width.
     *
     * From the Qt docs regarding the QMouseEvent::buttons() function:
     * "... For mouse release events this *excludes* the button that caused
     * the event."
     *
     * Whereas for QMouseEvent::button():
     * "... Returns the button that caused the event."
     *
     * For the above reason, use ::button() instead of ::buttons() here.
     */
    if (pEvent->button() == Qt::LeftButton) {
        int ypos = this->totalSlices * pEvent->pos().y() / this->height();
        emit this->sliceChanged(ypos);
    }
}

void Topogram::genData(QVector<Slice *> vecSlices, float angle)
{
    if (this->pRawData) {
        delete this->pRawData;
        this->pRawData = NULL;
    }

    this->pRawData = (float *)calloc(sizeof(float), 512 * 512);
    Q_ASSERT(this->pRawData);

    for (int z = 0; z < vecSlices.size(); z++) {
        const Slice *pzSlice = vecSlices.at(z);
        Q_ASSERT(pzSlice);

        float *pRawPixelData = pzSlice->getRawPixelData();
        Q_ASSERT(pRawPixelData);

        int w = pzSlice->getWidth();
        int h = pzSlice->getHeight();

        float a = tan(angle);
        float b1 = h;
        float b2 = -a*w;
        float step = (b1-b2) / 512.0;

        for (int idx = 0; idx < 512; idx++) {
            int cnt = 0;
            float luminance = 0.0;
            float b = b2 + idx * step;
            for (float x = 0.0; x < w; x++) {
                float y = a * x + b;
                int idx = ((int)y)*w + x;
                if (idx >= 0 && idx < 512*512) {
                    cnt++;
                    luminance += pRawPixelData[idx];
                } else {
                    if (cnt > 0) {
                        goto NEXT;
                    }
                }
            }
NEXT:;
            if (cnt == 0) { luminance = 0.0; cnt = 1; }
            int n = z*w + idx;
            this->pRawData[n] = MyMath::sstep(0.0, 0.2, luminance / cnt);
        }
    }
}
