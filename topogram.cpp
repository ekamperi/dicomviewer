#include "topogram.h"

#include <QDebug>
#include <QVBoxLayout>

Topogram::Topogram(QWidget *parent) : QWidget(parent)
{
    qDebug() << "-> " << Q_FUNC_INFO;
    pImage = NULL;
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

    this->pImage = new QImage(pNewData, width, height, QImage::Format_Indexed8);
    Q_ASSERT(this->pImage);

    QVector<QRgb> my_table;
    for (int i = 0; i < 256; i++) my_table.push_back(qRgb(i,i,i));
    this->pImage->setColorTable(my_table);

    this->pLayout = new QVBoxLayout(this);
    this->pLayout->setMargin(0);

    this->pLabel = new QLabel();
    this->pLabel->setMinimumSize(1,1);
    this->pLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pLayout->addWidget(this->pLabel);
}

Topogram::~Topogram()
{
}

void Topogram::resizeEvent(QResizeEvent *pEvent)
{
    QImage scaled = this->pImage->scaled(size());
    this->pLabel->setPixmap(QPixmap::fromImage(scaled));
}
