#pragma once


#include "Scene.h"
#include <ctime>

class DepthTracer
{
private:
	int PixSampleNums;
	Scene * pScene;
public:
	int Iterations;
	clock_t LoadStartTime;

	DepthTracer(Scene * _scene);
	Scene * GetScene();
	unsigned short * DepthTracerScanner(double camera_factor);
	unsigned short * DepthTracerScanner(double camera_factor, double * & pixDepthAccurate);
	float TraceRay(Ray & ray);
};
