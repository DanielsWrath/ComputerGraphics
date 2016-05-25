#ifndef QUAD_H_115209AE
#define QUAD_H_115209AE

#include "object.h"

class Quad : public Object
{
public:
    Quad(Point pos0, Point pos1, Point pos2, Point pos3): position0(pos0), position1(pos1), position2(pos2), position3(pos3) {
		}

    virtual Hit intersect(const Ray &ray);
    virtual Point getTexturePoint(const Point &p);

    const Point position0, position1, position2, position3;
};

#endif /* end of include guard: QUAD_H_115209AE */
