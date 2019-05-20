#pragma once

#include <algorithm>
#include "Vector3f.h"

using namespace std;

class Intersection;
class Ray;

class AixAlignBoundingBox
{
public:
	Point3f MinPoint, MaxPoint;//¶Ô½Çµã

	AixAlignBoundingBox() {};
	AixAlignBoundingBox(Point3f minPoint, Point3f maxPoint) :MinPoint(minPoint), MaxPoint(maxPoint) {};
	bool IsIntersect(Ray ray);
	static AixAlignBoundingBox MergeBox(const AixAlignBoundingBox & bbox1, const AixAlignBoundingBox & bbox2);

	inline float GetMinPointCoord(int axis) { return MinPoint.num[axis]; }
	inline float GetMaxPointCoord(int axis) { return MaxPoint.num[axis]; }	
};
