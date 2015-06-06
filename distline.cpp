#include "distline.h"

DistLine::DistLine()
{
}

DistLine::DistLine(const QPoint &p1, const QPoint &p2)
{
    this->setPoints(p1, p2);
}
