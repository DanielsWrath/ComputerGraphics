#include "plane.h"
#include <stdio.h>
#include <iostream>
#include <math.h>

/************************** Plane **********************************/
#define FINITE

Hit Plane::intersect(const Ray &ray)
{
	double denom, t, denomAbs;
	Point pos;
	Vector N, vec10 = position0 - position1, vec12 = position2 - position1;
	// calculate normal of plane
	N = vec10.cross(vec12);
	N.normalize();
	
	// calculate denominator and calculate the absolute value
	denom = N.dot(ray.D);
	if (denom < 0){
		denomAbs = denom * -1;
	} else{
		denomAbs = denom;
	}
	// if the absolute value < epsilon, no hit
	if (denomAbs > 1e-6) {
		// calculate distance to possible intersection
		t = (position0-ray.O).dot(N)/denom;
		// point of intersection
		pos = ray.O + ray.D*t;
		
		// if t is negative, no intersection
		if (t<0 )  return Hit::NO_HIT();
		// if the plane is set to finite, the intersection is compared to min and max coordinates. If it is too big or small, not hit
		#ifdef FINITE
		if(!(pos.x >= minX && pos.x <= maxX) || !(pos.y >= minY && pos.y <= maxY) || !(pos.z >= minZ && pos.z <= maxZ)) return Hit::NO_HIT();
		#endif
	}else{
		return Hit::NO_HIT();
	}
    return Hit(t,N);
}

Point Plane::getTexturePoint(const Point& p){
    return Point(-1,-1);
}