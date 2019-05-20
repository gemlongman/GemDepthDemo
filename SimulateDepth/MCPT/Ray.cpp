#include "Common.h"

Ray::Ray(Point3f _origin, Vector3f _direction, LightSourceType _source)
{
	RayPosition = _origin;
	RayDirection = normalize(_direction);
	SourceType = _source;

	LowerBound = EPS;
	UpBound = FLT_MAX;

	InverseDirection = 1.0 / RayDirection;
}
Point3f Ray::GetPoint(float t)
{
	return RayPosition + RayDirection * t;
}

bool Ray::IsInBounds(float tVal)
{
	return tVal <= UpBound && tVal >= LowerBound;
}

Vector3f Ray::GetReflect(const Vector3f& normal)
{
	return RayDirection - 2.0f * dot(normal, RayDirection) * normal;
}

//get refractDirection
bool Ray::GetIsRefractDirection(const Vector3f & normal, float nit, Vector3f & refractDirection)
{
	float ndoti = dot(normal, RayDirection);
	float k = 1.0f - nit * nit * (1.0f - ndoti * ndoti);
	if (k >= 0.0f) 
	{
		refractDirection = nit * RayDirection - normal * (nit * ndoti + sqrt(k));
		return true;
	}
	else // total internal reflection
	{
		return false;
	}
}
