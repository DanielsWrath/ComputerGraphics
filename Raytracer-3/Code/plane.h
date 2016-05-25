#ifndef PLANE_H_115209AE
#define PLANE_H_115209AE

#include "object.h"

class Plane : public Object
{
public:
    Plane(Point pos0, Point pos1, Point pos2, Point pos3, double minX, double maxX, double minY, double maxY, double minZ, double maxZ): position0(pos0), position1(pos1), position2(pos2), position3(pos3), minX(minX), maxX(maxX), minY(minY), maxY(maxY), minZ(minZ), maxZ(maxZ) {
		}

    virtual Hit intersect(const Ray &ray);
    virtual Point getTexturePoint(const Point &p);

    const Point position0, position1, position2, position3;
    const double minX, maxX, minY, maxY, minZ, maxZ;
};

#endif /* end of include guard: PLANE_H_115209AE */
