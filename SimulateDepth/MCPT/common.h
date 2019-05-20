#pragma once

#include "AixAlignBoundingBox.h"

static const Color3f BLACK = Color3f(0.0f, 0.0f, 0.0f);

class Scene;
class ObjModel;
class Triangle;


typedef struct {
	//pcl::PointXYZ p;
	//struct {
	//	double x;
	//	double y;
	//	double z;
	//} point;
	Point3f point;
	int id = -1;//start at 1
}IDPoint;

typedef struct {
	int point[4];//start at 1
}IDQuadrangle;

typedef struct {
	int point[3];//start at 1
}IDTriangle;

class Reflectance {
public:
	Vector3f ka;//环境光
	Vector3f kd;//漫反射
	Vector3f ks;//镜面反射
};

class Material
{
public:
	Material() :Shiness(10), RefractiveIndex(1.0) {};
	string name;
	Color3f Emission;
	Vector3f TransmissionFilter;//tf
	Reflectance Ref;
	double Shiness;//光泽度Ns
	double RefractiveIndex;//Ni
	bool IsIllumination;
};

enum LightSourceType { NONE, DIRECT, DIFFUSE_REFLECT, SPECULAR_REFLECT, TRANSMISSON };

class Ray
{
public:
	float UpBound;
	float LowerBound;
	Point3f RayPosition;
	Vector3f RayDirection;
	LightSourceType SourceType;
	Vector3f InverseDirection;//倒数
	Ray(Point3f _origin = Point3f(0.0, 0.0, 0.0), Vector3f _direction = Vector3f(0.0, 0.0, 0.0), LightSourceType _source = LightSourceType::NONE);

	Point3f GetPoint(float t);
	bool IsInBounds(float tVal);

	Vector3f GetReflect(const Vector3f & normal);
	bool GetIsRefractDirection(const Vector3f & normal, float nit, Vector3f & refractDirection);
};

class Intersection
{
public:
	Point3f point;//Intersection point
	Vector3f normal;
	Triangle * pBelongTriangle;
	Intersection() :pBelongTriangle(NULL) {};
};

enum AXIS
{
	X, Y, Z
};

class KdTree
{
public:
	KdTree() :MinPoint(NULL), MaxPoint(NULL) {}
	KdTree(vector<KdTree*> & basicObjList, int splitAxis = AXIS::Z);

	void BuildTree(vector<KdTree*> & basicObjList, int splitAxis = AXIS::Z);
	virtual Triangle * GeTriangle() { return NULL; }
	//int*  GeVertexIdx() { return NULL; }
	//int*  GeVertexNormalIdx() { return NULL; }
	virtual AixAlignBoundingBox GetAixAlignBoundingBox();
	virtual bool GetIsIntersect(Ray & ray, Intersection & intersection);
	

private:
	int SplitAxis;
	AixAlignBoundingBox box;
	KdTree * MinPoint;
	KdTree * MaxPoint;
	void partition(int axis, const vector<KdTree*> & all, vector<KdTree*> * lowVec, vector<KdTree*> * highVec);
};

class Triangle :public KdTree
{
public:
	Material material;
	Point3f TrianglePosition;
	Vector3f EdgeA, EdgeB;

	Triangle(ObjModel * _pmodel, int _vertex_idx[3], int _vertex_normal_idx[3]);
	void init();
	Vector3f getNormal(const Vector3f & point);
	Triangle * GeTriangle() { return this; }
	//int*  GeVertexIdx() { return VertexIdx; }
	//int*  GeVertexNormalIdx() { return VertexNormalIdx; }
	AixAlignBoundingBox GetAixAlignBoundingBox();
	bool GetIsIntersect(Ray & ray, Intersection & intersection);
	
	Vector3f normal;
	int VertexIdx[3];
	int VertexNormalIdx[3];
private:
	ObjModel * pBelongModel;
	Matrix4 barycentric;//质心
	
};

enum LightType
{
	SphereLightType, TriangleLightType, QuadLightType
};

class Light
{
public:
	LightType lightType;
	Point3f LightPosition;
	Vector3f EdgeA, EdgeB;//two edge of triangle or quad
	float Radius;
	Vector3f normal;
	Vector3f Emission;// color of Emission light
	float Area;// length(normal)
	int LightSampleNums;

	Light(Point3f _origin, float _r, Color3f _emission);
	Light(Point3f _origin, Vector3f A, Vector3f B, Color3f _emission);
	Light(Point3f _origin, Vector3f A, Vector3f B, Vector3f _normal, Color3f _emission);
	Color3f LightDirectIlluminationRender(Intersection& intersection, Ray& ray, Scene* scene);
};

//need not vt texture
class ObjModel
{
public:
	vector<Point3f> vertices;
	vector<KdTree * > pTriangles; // KdTree pointer Triangle
	vector<Light> lights;
	vector<Vector3f> normals;

	~ObjModel();// delete pTriangles

	bool LoadObjFile(const string objPath);
};

Point3f SphereSample(float radius);
Vector3f HemiSphereSample(Vector3f direction);
Vector3f HemiSphereImportanceSample(Vector3f up, float n);
Ray MonteCarloSample(Ray & ray, Intersection & intersection);

