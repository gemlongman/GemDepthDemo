#include "PathTracer.h"
#include <omp.h>
#include <iostream>
#include <string>


using namespace std;



PathTracer::PathTracer(Scene* _pScene, float _ambientF)
{
	srand(int(time(NULL)));
	pScene = _pScene;
	Iterations = 0;
	LoadStartTime = clock();
	PixSampleNums = PixSampleNum;//1 1 3
	RescursiveMaxDepth = MaxRescursiveDepth;//3 5 8
	MCSampleNums = MaxIterationsNum;
	Ambient = Color3f(_ambientF, _ambientF, _ambientF);
}

Scene * PathTracer::GetScene()
{
	return pScene;
}


Color3f PathTracer::TraceRay(Ray& ray, int cur_depth)
{
	if (cur_depth > RescursiveMaxDepth)
	{
		return BLACK;// material.Emission;
	}
		
	Intersection intersection;
	bool isIntersected = pScene->GetIsIntersect(ray, intersection);

	if (!isIntersected)
	{
		return BLACK;
	}
	else
	{
		// 
		//Vector3f rayLight2Intersection = intersection.point - ray.RayPosition;
		//float lengthRayLight2Intersection = length(rayLight2Intersection);

		Material & material = intersection.pBelongTriangle->material;
		Reflectance & ref = material.Ref;
		Ray newRay = MonteCarloSample(ray, intersection);

		Color3f indirectIllumination;
		if (newRay.SourceType != LightSourceType::NONE)
		{
			//indirectIllumination: return material.Emission + directIllumination + indirectIllumination;
			indirectIllumination = TraceRay(newRay, cur_depth + 1);

			Vector3f refk;
	
			switch (newRay.SourceType)
			{
			case LightSourceType::SPECULAR_REFLECT:
				refk = ref.ks;
				break;
			case LightSourceType::DIFFUSE_REFLECT:
				refk = ref.kd;
				break;
			//case LightSourceType::TRANSMISSON:
			//	break;
			}
			if (refk != BLACK)
			{
				indirectIllumination = refk * indirectIllumination;
			}
		}
		
		//Emission directIllumination indirectIllumination
		return ref.ka * Ambient + material.Emission + pScene->DirectIllumination(intersection, ray) +indirectIllumination;
	}
}

float clamp(float x) 
{ 
	if ( 0 > x ) 
	{
		return 0;
	}
	if (1 < x)
	{
		return 1;
	}
	return x; 
}

float * PathTracer::PathTracerRender()
{
	++Iterations;

	if (Iterations > MCSampleNums)
	{
		return pScene->pixColors;
	}

	cout << to_string(Iterations) + "th" << endl;

	int width = pScene->GetWidth();
	int height = pScene->GetHeight();

//#pragma unroll
#pragma omp parallel for schedule(dynamic, 1)
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Color3f color;
			Ray* rays = pScene->SceneGetRays(x, y, PixSampleNums);

			for (int k = 0; k < PixSampleNums; ++k)
			{
				
				Color3f result = TraceRay(rays[k]);
				//cout << "r " << result.r << " g " << result.g << " b " << result.b << endl;
				result.r = clamp(result.r);
				result.g = clamp(result.g);
				result.b = clamp(result.b);

				color += result;
			}

			color = color / PixSampleNums;
			delete[] rays;

			int index = y * width * 3 + x * 3;
			pScene->pixColors[index] = (pScene->pixColors[index] * (Iterations - 1) + color.r) / Iterations;
			pScene->pixColors[index + 1] = (pScene->pixColors[index + 1] * (Iterations - 1) + color.g) / Iterations;
			pScene->pixColors[index + 2] = (pScene->pixColors[index + 2] * (Iterations - 1) + color.b) / Iterations;
		}
	}

	return pScene->pixColors;
}


