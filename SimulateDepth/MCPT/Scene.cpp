#include "Scene.h"

Light::Light(Point3f _origin, float _r, Color3f _emission)
{
	lightType = SphereLightType;
	LightSampleNums = LightSampleNum;// 1 1 3
	LightPosition = _origin;
	Radius = _r;
	Emission = _emission ;
	//weight
	Area = Radius * Radius * dot(Emission, Vector3f(1, 1, 1) / 3 );// 1.333 * Radius * Radius * PI;// 1;
}

Light::Light(Point3f _origin, Vector3f A, Vector3f B, Color3f _emission)
{
	lightType = TriangleLightType;
	LightSampleNums = LightSampleNum;//1 1 3
	LightPosition = _origin;
	EdgeA = A;
	EdgeB = B;
	Emission = _emission;

	normal = cross(EdgeA, EdgeB) / 2;
	Area = length(normal);
#ifndef HIGH
	Area = Area * LightRate;
#endif // LOW or normal
	normal = normalize(normal);
}

Light::Light(Point3f _origin, Vector3f A, Vector3f B, Vector3f _normal, Color3f _emission) 
{
	lightType = QuadLightType;
	LightSampleNums = LightSampleNum;//1 1 3
	LightPosition = _origin;
	EdgeA = A;
	EdgeB = B;
	Emission = _emission;

	normal = normalize(_normal);
	Area = length( cross(EdgeA, EdgeB) );
#ifndef HIGH
	Area = Area * LightRate;
#endif // LOW or normal
}

Color3f Light::LightDirectIlluminationRender(Intersection & intersection, Ray & ray, Scene * scene)
{
	if (Emission == BLACK)
	{
		return BLACK;
	}

	float lightSamplesRate = 1.0f / LightSampleNums;
	Vector3f lightOriginPositon;
	Color3f rgbReturn;
	Material & intersectMaterial = intersection.pBelongTriangle->material;
	Reflectance & intersectMaterialRef = intersection.pBelongTriangle->material.Ref;

	if (SphereLightType == lightType)
	{
		lightSamplesRate = 2 * lightSamplesRate * Area / scene->averageLightWeight;//2 : hemisphere// weight:Area
	}
	else //if (QuadLightType == lightType)
	{
		lightSamplesRate *= PI;
	}

	for (int i = 0; i < LightSampleNums; i++)
	{
		Vector3f rayLight2Intersection = intersection.point - lightOriginPositon;
		float lengthRayLight2Intersection = length(rayLight2Intersection);
		float randX;
		float randY;

		if (TriangleLightType == lightType)
		{
			randX = (float)rand() / RAND_MAX;
			randY = (float)rand() / RAND_MAX * (1.0f - randX);
			lightOriginPositon = LightPosition + EdgeA * randX + EdgeB * randY;
		}
		else if (SphereLightType == lightType)//
		{
			randX = (float)rand() / RAND_MAX ;
			randX = acos( 1 - 2 * randX );//0-180
			randY = (float)rand() / RAND_MAX * 2 * PI;// 0-360
			lightOriginPositon = LightPosition + Vector3f( Radius * sin(randX) * cos(randY), Radius *  sin(randX) * sin(randY), Radius * cos(randX));
			normal = normalize( lightOriginPositon - LightPosition );

			//lightOriginPositon = LightPosition + SphereSample(Radius);
			//normal = normalize(lightOriginPositon - LightPosition);

			////HemiSphereSample
			//normal = HemiSphereSample(rayLight2Intersection);
			//lightOriginPositon = LightPosition + Radius * normal;

			//point
			//normal = normalize(intersection.point - lightOriginPositon);
			//lightOriginPositon = LightPosition + normal * Radius;
		}
		else if (QuadLightType == lightType)
		{
			randX = (float)rand() / RAND_MAX;
			randY = (float)rand() / RAND_MAX;
			lightOriginPositon = LightPosition + EdgeA * randX + EdgeB * randY;
		}
		else {
			cout << "error light type" << endl;
		}

		Ray shadowRay = Ray(intersection.point, -rayLight2Intersection);
		shadowRay.UpBound = lengthRayLight2Intersection;

		if (!scene->IsInShadow(shadowRay))
		{
			Vector3f normalizeRayLight2Intersection = normalize(rayLight2Intersection);;
			//float cosIAngle = dot(intersection.normal, -normalizeRayLight2Intersection);
			//float cosJAngle = dot(normal, normalizeRayLight2Intersection);
			float cosIAngle = max(dot(intersection.normal, -normalizeRayLight2Intersection), 0.0f);
			float cosJAngle = max(dot(normalizeRayLight2Intersection, normal), 0.0f);

			//Vij dAj dAi 
			float FormFactor = lightSamplesRate * cosIAngle * cosJAngle / (lengthRayLight2Intersection * lengthRayLight2Intersection * PI);
			Vector3f lightIntensity = Emission * FormFactor;

			if (intersectMaterialRef.ks != BLACK)
			{
				//specular
				Vector3f h = normalize(-normalizeRayLight2Intersection + ray.RayDirection);// H = L + V 
				float HN = dot(h, intersection.normal);
				if (HN > 0.0)
				{
					rgbReturn += lightIntensity * intersectMaterialRef.ks * pow(HN, intersectMaterial.Shiness);
				}
			}
			if (intersectMaterialRef.kd != BLACK)
			{
				//diffuse
				float NL = dot(-normalizeRayLight2Intersection, intersection.normal);
				if (NL > 0.0)
				{
					rgbReturn += lightIntensity * intersectMaterialRef.kd * NL;
				}
			}
		}
	}
	return rgbReturn;
}


Scene::Scene(string scenesName,double _camera_factor)
{
	pCamera.CameraFactor = _camera_factor;
	inputModel.LoadObjFile(scenesName);
	kdTree.BuildTree(inputModel.pTriangles);

	averageLightWeight = 0.0f;
	
}

Scene::~Scene()
{
	if (pixColors != NULL)
	{
		delete[] pixColors;
		pixColors = NULL;
	}

	if (pixDepth != NULL)
	{
		delete[] pixDepth;
		pixDepth = NULL;
	}

}

void Scene::SceneInit(string name)
{
	SceneName = name;
	width = pCamera.width;
	height = pCamera.height;

	if (pixColors != NULL)
	{
		delete[] pixColors;
	}
	pixColors = new float[width * height * 3];

	if (pixDepth != NULL)
	{
		delete[] pixDepth;
	}
	pixDepth = new unsigned short[width * height * 3];


	//init light
	for (int i = 0, len = this->lights.size(); i < len; ++i)
	{
		averageLightWeight += this->lights[i].Area;
	}
	averageLightWeight /= this->lights.size();
}

Ray * Scene::SceneGetRays(float x, float y, int pxSampleNum)
{
	float dx = 1.0f / width;
	float dy = 1.0f / height;

	if (pxSampleNum == 0)
	{
		Ray* rays = new Ray[1];
		rays[0] = pCamera.CameraGetRay(x * dx, y * dy);
		return rays;
	}
	else
	{
		Ray* rays = new Ray[pxSampleNum];
		for (int i = 0; i < pxSampleNum; ++i)
		{
			float randomX = (float)rand() / RAND_MAX;
			float randomY = (float)rand() / RAND_MAX;
			rays[i] = pCamera.CameraGetRay( (x + randomX) * dx, (y + randomY) * dy );
		}
		return rays;
	}

}

Ray * Scene::SceneGetDepthRays(float x, float y, int pxSampleNum)
{
	float dx = 1.0f / width;
	float dy = 1.0f / height;

	if (pxSampleNum == 0)
	{
		Ray* rays = new Ray[1];
		rays[0] = pCamera.CameraGetDepthRay(x * dx, y * dy);
		return rays;
	}
	else
	{
		Ray* rays = new Ray[pxSampleNum];
		for (int i = 0; i < pxSampleNum; ++i)
		{
			float randomX = (float)rand() / RAND_MAX;
			float randomY = (float)rand() / RAND_MAX;
			rays[i] = pCamera.CameraGetDepthRay((x + randomX) * dx, (y + randomY) * dy);
		}
		return rays;
	}

}

bool Scene::GetIsIntersect(Ray& ray, Intersection& intersection)
{
	return kdTree.GetIsIntersect(ray, intersection);
}

bool Scene::IsInShadow(Ray& ray)
{
	Intersection intersection;
	return kdTree.GetIsIntersect(ray, intersection);
}


Color3f Scene::DirectIllumination(Intersection& intersection, Ray& ray)
{
	Color3f color;
	for (int i = 0, len = this->lights.size(); i < len; ++i)
	{
		color += this->lights[i].LightDirectIlluminationRender(intersection, ray, this);
	}
	return color;
}


// #################################### Camera

void Camera::LookAt(float cameraPositionX, float cameraPositionY, float cameraPositionZ,
	float lookAtX, float lookAtY, float lookAtZ,
	float up_x, float up_y, float up_z)
{
	CameraPosition = Point3f(cameraPositionX, cameraPositionY, cameraPositionZ);
	LookAtPosition = Point3f(lookAtX, lookAtY, lookAtZ);
	oneStep = (double)length( LookAtPosition - CameraPosition) * 0.02;
	
	CameraDirection = normalize( LookAtPosition - CameraPosition );

	up = Vector3f(up_x, up_y, up_z);
	right = normalize(cross(CameraDirection, up));
	up = normalize(cross(right, CameraDirection));
	//cout << "up " << up.x << " " << up.y << " " << up.z << endl;
	//cout << "right " << right.x << " " << right.y << " " << right.z << endl;
}

void Camera::CalcViewPort(float _fov, int _width, int _height)
{
	fov = _fov;
	width = _width;
	height = _height;

	float aspect = (float)width / (float)height;

	viewHorizontal = right * 2 * tan( PI * fov / 360); // *x
	viewVertical = up * 2 * tan( aspect * PI * fov / 360); // *y
	viewDepth = CameraDirection;
	//cout << "viewHorizontal " << viewHorizontal.x << " " << viewHorizontal.y << " " << viewHorizontal.z << endl;
	//cout << "viewVertical " << viewVertical.x << " " << viewVertical.y << " " << viewVertical.z << endl;
	//cout << "viewDepth " << viewDepth.x << " " << viewDepth.y << " " << viewDepth.z << endl;
}

Ray Camera::CameraGetRay(float x, float y)// x,y => [0,1] 
{
	Vector3f direction = viewDepth + (x - 0.5f) * viewHorizontal + (y - 0.5f) * viewVertical;
	return Ray(CameraPosition, direction);
}

Ray Camera::CameraGetDepthRay(float x, float y)// x,y => [0,1] 
{
	Vector3f direction = viewDepth + (x - 0.5f) * viewHorizontal + (y - 0.5f) * viewVertical;
	return Ray(CameraPosition, direction);
}


//俯仰角 Angle of pitch //axis is right
void Camera::Pitch(double stepAngle)
{
	Matrix4 rotate;
	RotateArbitraryLine(rotate, Vector3f(LookAtPosition.x, LookAtPosition.y, LookAtPosition.z), Vector3f(LookAtPosition.x + right.x, LookAtPosition.y + right.y, LookAtPosition.z + right.z), stepAngle * oneAngle);
	CameraPosition = rotate * CameraPosition;
	CameraDirection = normalize(LookAtPosition - CameraPosition);

	//right 不变
	//up = Vector3f(0, 1, 0);
	//right = normalize(cross(CameraDirection, up));
	up = normalize(cross(right, CameraDirection));

	CalcViewPort(fov, width, height);
}

//水平旋转角 Horizontal turning //axis is (0 , 1, 0) at lookAt
void Camera::Turn(double stepAngle)
{
	// LookAtPosition.x, LookAtPosition.y +1, LookAtPosition.z
	// LookAtPosition.x, LookAtPosition.y, LookAtPosition.z
	Matrix4 rotate;
	RotateArbitraryLine(rotate, Vector3f(LookAtPosition.x, LookAtPosition.y, LookAtPosition.z), Vector3f(LookAtPosition.x, LookAtPosition.y + 1, LookAtPosition.z), stepAngle * oneAngle);
	CameraPosition = rotate * CameraPosition;
	CameraDirection = normalize(LookAtPosition - CameraPosition);
	
	//up 不变
	up = Vector3f(0, 1, 0);
	right = normalize(cross(CameraDirection, up));
	up = normalize(cross(right, CameraDirection));

	CalcViewPort(fov, width, height);
}

//far near
void Camera::MoveForward(double steps) 
{
	CameraPosition = CameraPosition + steps * oneStep * CameraDirection;
}