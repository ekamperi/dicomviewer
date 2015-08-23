#include "slice.h"
#include "examdetails.h"
#include <QDebug>

Slice::Slice(QString filename, unsigned int index)
{
//    qDebug() << Q_FUNC_INFO;

    /* Index to numerically identify a slice */
    // XXX This will be overriden by slice location reordering
    this->index = index;

    /* Create a DicomFile object to dissect the input file */
    this->pDicomFile = new DicomFile();
    Q_ASSERT(this->pDicomFile);

    pDicomFile->loadDicomFile(filename);

    /* Extract the raw pixel data from the DICOM file */
    this->pRawPixelData = pDicomFile->getUncompressedData();
    this->width = pDicomFile->getWidth();
    this->height = pDicomFile->getHeight();
    this->format = pDicomFile->getFormat();
    this->maxPixel = pDicomFile->getMaxPixel();

    /* Extract current window/width */
    this->pHUConverter = pDicomFile->getHUConverter();

    /* Also extract the examination details (patients name, age, etc) */
    this->examDetails = pDicomFile->getExamDetails();
}

Slice::~Slice()
{
    Q_ASSERT(this->pDicomFile);
    delete this->pDicomFile;
}

void Slice::normalizePixels(float globalMaxPixel)
{
    //Q_ASSERT(maxPixel > 0.0);
//    qDebug() << Q_FUNC_INFO << " globalMaxPixel =" << globalMaxPixel;

    this->globalMaxPixel = globalMaxPixel;

    for (unsigned long i = 0; i < this->width * this->height; i++) {
        this->pRawPixelData[i] = this->pRawPixelData[i] / globalMaxPixel;
    }

    /* Also normalize the window center/width */
    this->pHUConverter->normalize(globalMaxPixel);
}

QPair<float, float> Slice::getDefaultWindowLevelWidth(void) const
{
    // XXX
    return QPair<float, float>(0.0, 0.5);
}

/*
 * This function is used in conjuction with qsort() on the vector
 * holding the slices, in order to sort them by their slice location value.
 */
bool Slice::comparator(const Slice *pLeft, const Slice *pRight)
{
    Q_ASSERT(pLeft);
    Q_ASSERT(pRight);
    int left  =  pLeft->getExamDetails().getSliceLocation().toInt();
    int right = pRight->getExamDetails().getSliceLocation().toInt();
    return left > right;    //XXX
}
