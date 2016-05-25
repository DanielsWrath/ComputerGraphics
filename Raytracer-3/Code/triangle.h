#ifndef TRIANGLE_H_115209AE
#define TRIANGLE_H_115209AE

#include "object.h"

class Triangle : public Object
{
public:
    Triangle(Point pos0, Point pos1, Point pos2): position0(pos0), position1(pos1), position2(pos2) {
		}

    virtual Hit intersect(const Ray &ray);
    virtual Point getTexturePoint(const Point &p);

    const Point position0, position1, position2;
};

#endif /* end of include guard: TRIANGLE_H_115209AE */
