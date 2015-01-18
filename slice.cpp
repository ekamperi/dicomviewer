#include "slice.h"
#include "examdetails.h"
#include <QDebug>

Slice::Slice(QString filename, unsigned int index)
{
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

    /* Also extract the examination details (patients name, age, etc) */
    this->examDetails = pDicomFile->getExamDetails();

    /* Whether this slice is selected */
    this->m_isSelected = false;
}

Slice::~Slice()
{
    delete this->pDicomFile;
}
