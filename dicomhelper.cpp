#include "dicomhelper.h"

#include <QDebug>

DicomHelper::DicomHelper()
{

}

DicomHelper::~DicomHelper()
{

}

QString DicomHelper::getDcmTagKeyAsQString(DcmDataset *pDcmDataset, const DcmTagKey &dcmTagKey)
{
    Q_ASSERT(pDcmDataset);

    /* Retrieve the value of 'dcmTagKey', e.g. 'dcmTagKey' may be DCM_PatientName */
    OFString result;
    OFCondition status = pDcmDataset->findAndGetOFString(dcmTagKey, result);
    if (status.good()) {
        return QString(result.c_str());
    } else {
        qDebug() << status.text();
        return QString("N/A");
    }
}
