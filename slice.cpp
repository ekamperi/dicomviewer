#include "slice.h"
#include "examdetails.h"
#include <QDebug>

Slice::Slice(QString filename, unsigned int index)
{
//    qDebug() << Q_FUNC_INFO;

    /* Index to numerically identify a slice */
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

    /* Whether this slice is selected */
    this->m_isSelected = false;
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

void Slice::setWindow(HUWindows::window huWindow)
{
    qDebug() << Q_FUNC_INFO;

    this->huWindow = huWindow;

    QPair<float, float> tMinMax =
            this->pHUConverter->getNormalizedRangeFromTemplate(this->huWindow);

    /*
     * Notify any GL widgets that are connected to us, that they need
     * to repaint the data because thw window/level has changed.
     */
    emit this->iNeedRepaint(tMinMax.first, tMinMax.second);
}
