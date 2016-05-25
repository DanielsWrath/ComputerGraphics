#include "triangle.h"
#include <stdio.h>
#include <iostream>
#include <math.h>

/************************** Triangle **********************************/

Hit Triangle::intersect(const Ray &ray)
{
	double t;
	Point pos;
	Vector N, vec10 = position1 - position0, vec12 = position2 - position0;
	
	Vector P = ray.D.cross(vec12);
	// calculate determiner
	double det = vec10.dot(P);
	// if determiner is close to zero, no hit
	if(det > -0.000001 && det < 0.000001) return Hit::NO_HIT();
	
	//calculate inverse of determiner
	double inv_det = 1.f/det;
	
	// distance from ray to position0
	Vector T = ray.O - position0;
	double u = T.dot(P) * inv_det;
	
	if (u < 0.f || u > 1.f) return Hit::NO_HIT();
	
	Vector Q = T.cross(vec10);
	
	double v = ray.D.dot(Q) * inv_det;
	
	// another check if v is not too small or u+v too big
	if(v < 0.f || u+v > 1.f) return Hit::NO_HIT();
	
	// calculate distance to intersection point
	t = vec12.dot(Q) * inv_det;
	
	// if t < epsilon, no hit
	if(t <= 0.000001){
		return Hit::NO_HIT();
	}
	
	// normal calculated
	N = vec10.cross(vec12);
	N.normalize();
	

    return Hit(t,N);
}

Point Triangle::getTexturePoint(const Point& p){
    return Point(-1,-1);
}
