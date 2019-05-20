#pragma once

#include "Scene.h"
#include <ctime>

class PathTracer
{
private:
	int PixSampleNums;
	int MCSampleNums;
	int RescursiveMaxDepth;
	Scene * pScene;
	Color3f Ambient;
public:
	int Iterations;
	clock_t LoadStartTime;

	PathTracer(Scene * _scene, float _ambientF);
	Scene * GetScene();
	float * PathTracerRender();
	Color3f TraceRay(Ray & ray, int curDepth = 0);
};
