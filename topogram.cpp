#include "mymath.h"
#include "topogram.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>

Topogram::Topogram(QWidget *parent) : QWidget(parent)
{
    qDebug() << "-> " << Q_FUNC_INFO;
    pImage = NULL;

    // XXX
    this->sliceIndex = 20;
}

Topogram::Topogram(float *pRawData, int width, int height, int sliceIndex,
                   QWidget *parent) : QWidget(parent)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "width =" << "height =" << width << height;

    this->pRawData = pRawData;
    this->sliceIndex = sliceIndex;
    this->totalSlices = height;

    // Convert float data to uchar
    unsigned long nPixels = width * height;
    unsigned char *pNewData = new unsigned char[nPixels];
    Q_ASSERT(pNewData);

    for (unsigned long i = 0; i < nPixels; i++) {
        pNewData[i] = (unsigned char) (this->pRawData[i] * 255);
    }

    this->pImage = new QImage(pNewData, width, height, QImage::Format_Indexed8);
    Q_ASSERT(this->pImage);

    QVector<QRgb> my_table;
    for (int i = 0; i < 256; i++) my_table.push_back(qRgb(i,i,i));
    this->pImage->setColorTable(my_table);

    this->pLayout = new QVBoxLayout(this);
    this->pLayout->setMargin(0);
    this->setGeometry(0, 0, width, 5*height);
 }

Topogram::~Topogram()
{
}

void Topogram::mouseMoveEvent(QMouseEvent *pEvent)
{
    qDebug() << Q_FUNC_INFO;

    if (pEvent->buttons() & Qt::RightButton) {
        float x = pEvent->localPos().x();
        float y = pEvent->localPos().y();

        float width = y / ((float) this->height());
        float center = x / ((float) this->width());

        if (width < 0.0) width = 0.0;
        if (width > 1.0) width = 1.0;
        if (center < 0.0) center = 0.0;
        if (center > 1.0) center = 1.0;

        this->tmin = center - width/2;
        this->tmax = center + width/2;

        if (this->tmin < 0.0) this->tmin = 0.0;
        if (this->tmax > 1.0) this->tmax = 1.0;
    }

    qDebug() << this->tmin << this->tmax;
    for (int i = 0; i < this->width()*this->height(); i++) {
        this->pRawData[i] = MyMath::sstep(this->tmin, this->tmax, this->pRawData[i]);
    }
    this->update();
}

void Topogram::paintEvent(QPaintEvent *pEvent)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << this->sliceIndex << this->totalSlices;

    QImage scaled = this->pImage->scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //this->pLabel->setPixmap(QPixmap::fromImage(scaled));

    QPainter painter;

    painter.begin(this);
    painter.drawPixmap(0,0,width(), height(), QPixmap::fromImage((scaled)));
    float ypos = ((float)(this->sliceIndex) / this->totalSlices) * height();
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::red);
    painter.setPen(pen);
    painter.drawLine(0, ypos, width(), ypos);
    painter.end();
}
