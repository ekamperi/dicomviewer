#ifndef DICOMHELPER_H
#define DICOMHELPER_H

#include <QString>

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

class DicomHelper
{
public:
    DicomHelper();
    ~DicomHelper();

    static QString getDcmTagKeyAsQString(DcmDataset *pDcmDataset, const DcmTagKey &dcmTagKey);
};

#endif // DICOMHELPER_H
