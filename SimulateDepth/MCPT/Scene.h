#pragma once

#include <vector>
#include "pch.h"
#include "Common.h"


class BasicObj;
class KdTree;

class Camera
{
public:
	int width, height;
	float fov;
	double CameraFactor;

	//Camera position toward upward
	void LookAt(float eye_x, float eye_y, float eye_z,
		float center_x, float center_y, float center_z,
		float up_x=0, float up_y=1, float up_z=0);
	void CalcViewPort(float _fov, int _width, int _height);
	Ray CameraGetRay(float x, float y);
	Ray CameraGetDepthRay(float x, float y);

	//¸©Ñö½Ç Angle of pitch
	void Pitch(double stepAngle = 1);
	//Ë®Æ½Ðý×ª½Ç Horizontal turning
	void Turn( double stepAngle=1);
	//far near
	void MoveForward(double steps);
	// need not tilt ÇãÐ±

	Point3f CameraPosition;
	Point3f LookAtPosition;
	Vector3f CameraDirection;
	Vector3f front, up, right;
	Vector3f viewHorizontal, viewVertical, viewDepth;
private:
	const double oneAngle = PI / 180 ;
	double oneStep;
};

class Scene
{
public:
	ObjModel inputModel;
	Camera pCamera;
	std::vector<Light> lights;

	std::string SceneName;
	
	float averageLightWeight;

	float * pixColors;
	unsigned short * pixDepth;
	//double * pixDepthAccurate;

	Scene(std::string scenesName, double _camera_factor);
	~Scene();

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	void SceneInit(std::string name="");
	Ray * SceneGetRays(float x, float y, int pxSampleNum);
	Ray * SceneGetDepthRays(float x, float y, int pxSampleNum);
	bool GetIsIntersect(Ray & ray, Intersection & intersection);
	bool IsInShadow(Ray & ray);//is Occlusion between
	Color3f DirectIllumination(Intersection & intersection, Ray & ray);

private:
	int width, height;
	
	KdTree kdTree;
};

