#ifndef REALPLANE_H_115209AE
#define REALPLANE_H_115209AE

#include "object.h"

class RealPlane : public Object
{
public:
    RealPlane(Point position, Vector normal): position(position), normal(normal) {
		}

    virtual Hit intersect(const Ray &ray);
    virtual Point getTexturePoint(const Point &p);

    const Point position;
    const Vector normal;
};

#endif /* end of include guard: REALPLANE_H_115209AE */
