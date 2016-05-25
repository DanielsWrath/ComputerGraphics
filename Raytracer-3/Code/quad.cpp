#include "triangle.h"
#include "quad.h"
#include <stdio.h>
#include <iostream>
#include <math.h>

/************************** Quad **********************************/

Hit Quad::intersect(const Ray &ray)
{
	bool changed = false;
	// Create two triangles using points p0,p1,p2 and p1,p2,p3
	Triangle *t0 = new Triangle(position0, position1, position2), *t1 = new Triangle(position1, position2, position3);
	Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
	// check if a triangle is hit and which is closest
	Hit h0(t0->intersect(ray)), h1(t1->intersect(ray));
    if(h0.t < min_hit.t) min_hit = h0; changed = true;
    if(h1.t < min_hit.t) min_hit = h1; changed = true;
    // if none is hit, return no hit
    if(!changed) return Hit::NO_HIT();
    
    // return hit
    return min_hit;
}

Point Quad::getTexturePoint(const Point& p){
    return Point(-1,-1);
}
