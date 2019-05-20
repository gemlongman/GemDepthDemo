#include "DepthTracer.h"
#include <omp.h>
#include <iostream>
#include <string>


using namespace std;



DepthTracer::DepthTracer(Scene* _pScene)
{
	srand(int(time(NULL)));
	pScene = _pScene;
	LoadStartTime = clock();
	PixSampleNums = PixSampleNum;//1 1 3
}

Scene * DepthTracer::GetScene()
{
	return pScene;
}


float DepthTracer::TraceRay(Ray& ray)
{
	float lengthRay2Intersection = 0.0;
	Intersection intersection;

	bool isIntersected = pScene->GetIsIntersect(ray, intersection);
	if (isIntersected)
	{
 
		Vector3f ray2Intersection = intersection.point - ray.RayPosition;
		lengthRay2Intersection = length(ray2Intersection);
	}
	return lengthRay2Intersection;
}


unsigned short * DepthTracer::DepthTracerScanner(double camera_factor)
{
	int width = pScene->GetWidth();
	int height = pScene->GetHeight();
	Vector3f cameraDirection = pScene->pCamera.CameraDirection;
#pragma omp parallel for schedule(dynamic, 1)
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float curDepth=0.0;
			Ray* rays = pScene->SceneGetDepthRays(x, y, PixSampleNums);

			for (int k = 0; k < PixSampleNums; ++k)
			{

				float result = TraceRay(rays[k]);
				result = result * dot(cameraDirection, normalize(rays[k].RayDirection));// ¾°Éî = dis * cos<>
				curDepth += result;
			}

			curDepth = curDepth / PixSampleNums;
			delete[] rays;

			int index = y * width + x;
			pScene->pixDepth[index] = curDepth * camera_factor;
			//cout << " \t " << pScene->pixDepth[index];
		}
		//cout << endl;
	}

	return pScene->pixDepth;
}


unsigned short * DepthTracer::DepthTracerScanner(double camera_factor, double * & pixDepthAccurate)
{
	int width = pScene->GetWidth();
	int height = pScene->GetHeight();
	Vector3f cameraDirection = pScene->pCamera.CameraDirection;
#pragma omp parallel for schedule(dynamic, 1)
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float curDepth = 0.0;
			Ray* rays = pScene->SceneGetDepthRays(x, y, PixSampleNums);

			for (int k = 0; k < PixSampleNums; ++k)
			{

				float result = TraceRay(rays[k]);
				result = result * dot(cameraDirection, normalize(rays[k].RayDirection));// ¾°Éî = dis * cos<>
				curDepth += result;
			}

			curDepth = curDepth / PixSampleNums;
			delete[] rays;

			int index = y * width + x;
			pScene->pixDepth[index] = pixDepthAccurate[y * width + width- x] = curDepth * camera_factor;
			//cout << " \t " << pScene->pixDepth[index];
		}
		//cout << endl;
	}

	return pScene->pixDepth;
}

