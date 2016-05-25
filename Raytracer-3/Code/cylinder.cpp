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

#include "cylinder.h"
#include <stdio.h>
#include <iostream>
#include <math.h>

#define PI 3.1415926
#define LAMBDA 0.00001

/************************** Cylinder **********************************/

Hit Cylinder::intersect(const Ray &ray)
{
    //cout << "intersectCylinder" << endl;
    double t0, t1, y0, y1;
    Point deltaP = position0 - position1;
    Point center = position0 + (deltaP/2);
    Vector Va = (position1-position0)/(position1-position0).length();
    double t = 0;
    Vector N = Vector(0,0,0);
    Vector newO = ray.O - position0 ;
    double l = r;
    //Caps
    //q = p + vt
    //(va *(q- p1)) = 0, (q- p1)2 < r2 bottom cap
    //(va *(q-p2))=0,(q-p2)2<r2 topcap
    
    // a=xD2+yD2, b=2xExD+2yEyD, and c=xE2+yE2-1.
    double a = pow(ray.D.x,2) - pow(ray.D.y,2)+ pow(ray.D.z,2);
    double b = 2 * newO.x * ray.D.x - 2 * newO.y * ray.D.y + 2 * newO.z * ray.D.z;
    double c = pow(newO.x,2) - pow(newO.y,2)+ pow(newO.z,2);
    
    //double a = pow(ray.D.x,2)+ pow(ray.D.z,2);
    //double b = 2 * newO.x * ray.D.x  + 2 * newO.z * ray.D.z;
    //double c = pow(newO.x,2) + pow(newO.z,2) - 1;
    
    
    double abc = b * b - 4 * a * c;

    if(abc < 0){
        // doesn't hit side of cylinder, maybe going straight through caps
        return Hit::NO_HIT();
    }
    
    t0 = (-b+sqrt(abc))/(2*a);
    t1 = (-b-sqrt(abc))/(2*a);
    if (t0 > t1) {
        double  change = t0;
        t0 = t1;
        t1 = change;
    }
    
    y0 = ray.O.y + t0 * ray.D.y;
    y1 = ray.O.y + t1 * ray.D.y;
    cout << y0 << endl;
    cout << y1 << endl;
    
    if ((y0 > l && y1 > l) || (y0 < -l && y1 < l)) {
        //MISS
        return Hit::NO_HIT();
    }
    else if (y0 <-l && y1 >-l) {
        //HIT CAP -l
        float th = t0 + (t1-t0) * (y0+1) / (y0-y1);
        if (th<=0) return Hit::NO_HIT();
        
        t = th;
        N = Vector(0, -1, 0);
    }
    else if ( y0 >l && y1 <l) {
        //HIT CAP +1
        float th = t0 + (t1-t0) * (y0-1) / (y0-y1);
        if (th<=0) return Hit::NO_HIT();
        
        t = th;
        N = Vector(0, 1, 0);
        
    }
    else if (y0 <l && y0 >-l) {
        //HIT SIDES
        if (t0<=0) return Hit::NO_HIT();
        
        t = t0;
        Point pos = ray.O + ray.D * t0;
        N = Vector(0, -1, 0);
        N = Vector(pos.x, 0, pos.z);
    }
    
    N = N.normalized();

    cout << "cylinder hit" << endl;
    return Hit(t,N);
}

Point Cylinder::getTexturePoint(const Point &p){
    return Point(-1,-1,-1);
}
