#include "denscircle.h"
#include "math.h"

DensCircle::DensCircle(const QPoint &startPoint, const QPoint &endPoint)
{
    m_center = startPoint;

    float dx = endPoint.x() - startPoint.x();
    float dy = endPoint.y() - startPoint.y();
    m_radius = sqrt(dx*dx + dy*dy);
}
