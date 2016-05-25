#include "realplane.h"
#include <stdio.h>
#include <iostream>
#include <math.h>

/************************** RealPlane **********************************/
#define LAMBDA 0.00001

Hit RealPlane::intersect(const Ray &ray)
{
	// Normalize normal to be sure
	Vector N = normal.normalized();
	// Check if it hits	
	
	if(fabs(N.dot(ray.D)) < LAMBDA) return Hit::NO_HIT();
	
    return Hit((position-ray.O).dot(N)/N.dot(ray.D),N);
}

Point RealPlane::getTexturePoint(const Point& p){
    return Point(-1,-1);
}
