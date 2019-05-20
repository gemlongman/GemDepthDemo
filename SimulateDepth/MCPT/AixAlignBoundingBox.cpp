#include "Common.h"

AixAlignBoundingBox AixAlignBoundingBox::MergeBox(const AixAlignBoundingBox& box1, const AixAlignBoundingBox& box2)
{
	Point3f pt1, pt2;

	pt1.x = min(box1.MinPoint.x, box2.MinPoint.x);
	pt1.y = min(box1.MinPoint.y, box2.MinPoint.y);
	pt1.z = min(box1.MinPoint.z, box2.MinPoint.z);

	pt2.x = max(box1.MaxPoint.x, box2.MaxPoint.x);
	pt2.y = max(box1.MaxPoint.y, box2.MaxPoint.y);
	pt2.z = max(box1.MaxPoint.z, box2.MaxPoint.z);

	return AixAlignBoundingBox(pt1, pt2);
}

bool AixAlignBoundingBox::IsIntersect(Ray ray)
{
	Vector3f inverseDirection = ray.InverseDirection;

	float t1 = (MinPoint.x - ray.RayPosition.x)*inverseDirection.x;//��box��yozƽ���ཻ��x��С
	float t2 = (MaxPoint.x - ray.RayPosition.x)*inverseDirection.x;

	float t3 = (MinPoint.y - ray.RayPosition.y)*inverseDirection.y;//��box��xozƽ���ཻ��y��С
	float t4 = (MaxPoint.y - ray.RayPosition.y)*inverseDirection.y;

	float t5 = (MinPoint.z - ray.RayPosition.z)*inverseDirection.z;//��box��xoyƽ���ཻ��z��С
	float t6 = (MaxPoint.z - ray.RayPosition.z)*inverseDirection.z;

	//������box��ǰ��2������ľ���t
	float lowerBound = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float upBound = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	if (upBound < 0)
	{
		return false;
	}
	if (lowerBound > upBound)
	{ 
		return false;
	}
	return (lowerBound < ray.UpBound && upBound > ray.LowerBound);

}
