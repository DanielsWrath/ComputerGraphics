//
//  Framework for a raytracer
//  File: sphere.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "sphere.h"
#include <stdio.h>
#include <iostream>
#include <math.h>

#define PI 3.1415926

/************************** Sphere **********************************/

Hit Sphere::intersect(const Ray &ray)
{
    double distanceToCenter, distanceIntersect, t, distanceHit0, distanceHit1, distanceFromOrigin;
    Point C = position;
	
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    * 
    * Insert calculation of ray/sphere intersection here. 
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/
	

    // place holder for actual intersection calculation
    
	//Distance from ray.O to C
    distanceToCenter = sqrt(pow((ray.O.x - C.x),2) + pow((ray.O.y - C.y),2) + pow((ray.O.z - C.z),2));
    //Distance to the point (possibly) in sphere
    distanceFromOrigin = (C - ray.O).dot(ray.D);
    if (distanceFromOrigin < 0) return Hit::NO_HIT();
	//Distance between the point of the ray and the center of the sphere
    distanceIntersect = sqrt((C-ray.O).dot(C-ray.O) - pow(distanceFromOrigin,2));
    if (distanceIntersect > r){
		// NO HIT
        return Hit::NO_HIT();
	}
	if (distanceIntersect == r) {
		 //Hit at the edge of the sphere (1 intersection point), point is intersection
        t = distanceIntersect;
	}
	else {
		//Hit at any other point in the sphere (2 intersection points)
        double b = 2*(ray.D.x * (ray.O.x-C.x) + ray.D.y * (ray.O.y - C.y) + ray.D.z * (ray.O.z - C.z));
        double c = pow(ray.O.x - C.x , 2) + pow(ray.O.y - C.y, 2) + pow(ray.O.z - C.z,2) - pow(r,2);
        double d = pow(b,2)-4*c;
		// use abc formula to calculate both solutions, the distance is the smallest one.
        distanceHit0  = (-b - sqrt(d)) * 0.5;
        distanceHit1 =  (-b + sqrt(d)) * 0.5;
        if (distanceHit0 < distanceHit1){
            t = distanceHit0;
        }else {
            t = distanceHit1;
        }
	}
	//    vec2 longitudeLatitude = vec2((atan(textCoords.x, textCoords.z) / 3.1415926 + 1.0) * 0.5, (1 - acos(textCoords.y) / 3.1415926 ));

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    * 
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/
	// calculate the normal
    Vector N = ((ray.O + (ray.D * t)) - C);
    N = N.normalized();

    return Hit(t,N);
}

//calculate uv coordinates and return them, also use rotation on x axis
Point Sphere::getTexturePoint(const Point &p){
    double angleRad = angle* (PI/180);
    Point C = Point((p-position));
    C.normalize();
    double U = 0.5 - ( (atan2(C.z, C.x)-angleRad)/(PI*2));
    double V = 0.5 - (asin(C.y)/PI) ;
    return Point(U,V);
}
