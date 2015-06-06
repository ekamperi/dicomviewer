#ifndef DISTLINE_H
#define DISTLINE_H

#include <QLine>

#include "selectable.h"

class DistLine : public QLine, public Selectable
{
public:
    DistLine();
    DistLine(const QPoint &, const QPoint &);
};

#endif // DISTLINE_H
