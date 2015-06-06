#ifndef DENSITYCIRCLE_H
#define DENSITYCIRCLE_H

#include <QLine>

#include "selectable.h"

class DensCircle : public Selectable
{
public:
    DensCircle();
    DensCircle(const QPoint &startPoint, const QPoint &endPoint);

    QPoint center(void) const { return this->m_center; }
    int radius(void) const { return this->m_radius; }
private:
    QPoint m_center;
    int m_radius;
};

#endif // DENSITYCIRCLE_H
