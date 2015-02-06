#include "topogram.h"

#include <QDebug>

Topogram::Topogram(QWidget *parent) : QWidget(parent)
{
    qDebug() << "-> " << Q_FUNC_INFO;
}

Topogram::Topogram(float *pData, int width, int height,
                   QWidget *parent) : QWidget(parent)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "width =" << "height =" << width << height;

    // Convert float data to uchar
    unsigned long nPixels = width * height;
    unsigned char *pNewData = new unsigned char[nPixels];
    Q_ASSERT(pNewData);

    for (unsigned long i = 0; i < nPixels; i++) {
        pNewData[i] = (unsigned char) (pData[i] * 255);
      //  qDebug() << pNewData[i] << " ";
    }

    QImage *qi = new QImage(pNewData, width, height, QImage::Format_Indexed8);
    Q_ASSERT(qi);

    QVector<QRgb> my_table;
    for(int i = 0; i < 256; i++) my_table.push_back(qRgb(i,i,i));
    qi->setColorTable(my_table);

    this->pLabel = new QLabel();
    this->pLabel->setPixmap(QPixmap::fromImage(*qi));
}

Topogram::~Topogram()
{

}

void Topogram::resizeEvent (QResizeEvent *pEvent)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "w =" << this->width() << "h =" << this->height();
    this->pLabel->setFixedSize(this->width(), this->height());
    this->pLabel->update();
}
