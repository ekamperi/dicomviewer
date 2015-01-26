#include "slice.h"
#include "examdetails.h"
#include <QDebug>

Slice::Slice(QString filename, unsigned int index)
{
    qDebug() << Q_FUNC_INFO;

    /* Index to numerically identify a slice */
    this->index = index;

    /* Create a DicomFile object to dissect the input file */
    this->pDicomFile = new DicomFile();
    pDicomFile->loadDicomFile(filename);

    /* Extract the raw pixel data from the DICOM file */
    this->pRawPixelData = pDicomFile->getUncompressedData();
    this->width = pDicomFile->getWidth();
    this->height = pDicomFile->getHeight();
    this->format = pDicomFile->getFormat();
    this->maxPixel = pDicomFile->getMaxPixel();

    /* Extract current window/width */
    this->defHUWindowWidth = pDicomFile->getDefaultHUF();

    /* Also extract the examination details (patients name, age, etc) */
    this->examDetails = pDicomFile->getExamDetails();

    /* Whether this slice is selected */
    this->m_isSelected = false;
}

Slice::~Slice()
{
    delete this->pDicomFile;
}

void Slice::normalizePixels(float maxPixel)
{
    Q_ASSERT(maxPixel > 0.0);

    for (unsigned long i = 0; i < this->width * this->height; i++) {
        this->pRawPixelData[i] = this->pRawPixelData[i] / maxPixel;
    }

    /* Also normalize the window center/width */
    this->defHUWindowWidth.normalize(maxPixel);
}

QPair<float, float> Slice::getDefaultWindowWidth(void) const
{
    return this->defHUWindowWidth.getDefaultNormalizedRange();
}
