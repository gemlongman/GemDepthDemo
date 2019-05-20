#include "Common.h"
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>

using namespace std;

#ifndef HIGH
static int lightCount = 0;
#endif

//--------------------------------- Triangle
Triangle::Triangle(ObjModel* _pmodel, int _vertexIdx[3], int _vertexNormalIdx[3])
{
	pBelongModel = _pmodel;
	memcpy(VertexIdx, _vertexIdx, sizeof(_vertexIdx) * 3);
	memcpy(VertexNormalIdx, _vertexNormalIdx, sizeof(_vertexNormalIdx) * 3);
}

void Triangle::init()
{
	Point3f & pt0 = pBelongModel->vertices[VertexIdx[0]];
	Point3f & pt1 = pBelongModel->vertices[VertexIdx[1]];
	Point3f & pt2 = pBelongModel->vertices[VertexIdx[2]];

	TrianglePosition = pt0;
	EdgeA = pt1 - pt0;
	EdgeB = pt2 - pt0;
	normal = normalize(cross(EdgeA, EdgeB));

	barycentric = Matrix4(pt0, pt1, pt2);
	barycentric.inverse();
}

Vector3f Triangle::getNormal(const Vector3f& point)//average Normal
{
	Vector3f averageNormal = barycentric * point;

	averageNormal.x = abs(averageNormal.x);
	averageNormal.y = abs(averageNormal.y);
	averageNormal.z = abs(averageNormal.z);

	return normalize(
		averageNormal.x * (pBelongModel->normals[VertexNormalIdx[0]]) +
		averageNormal.y * (pBelongModel->normals[VertexNormalIdx[1]]) +
		averageNormal.z * (pBelongModel->normals[VertexNormalIdx[2]]));
}

AixAlignBoundingBox Triangle::GetAixAlignBoundingBox()
{
	Point3f MinPoint, MaxPoint;
	Point3f & pt0 = pBelongModel->vertices[VertexIdx[0]];
	Point3f & pt1 = pBelongModel->vertices[VertexIdx[1]];
	Point3f & pt2 = pBelongModel->vertices[VertexIdx[2]];

	MinPoint.x = min(pt0.x, min(pt1.x, pt2.x));
	MinPoint.y = min(pt0.y, min(pt1.y, pt2.y));
	MinPoint.z = min(pt0.z, min(pt1.z, pt2.z));

	MaxPoint.x = max(pt0.x, max(pt1.x, pt2.x));
	MaxPoint.y = max(pt0.y, max(pt1.y, pt2.y));
	MaxPoint.z = max(pt0.z, max(pt1.z, pt2.z));

	return AixAlignBoundingBox(MinPoint, MaxPoint);
}


//get intersection
bool Triangle::GetIsIntersect(Ray & ray, Intersection & intersection)
{
	if (abs(dot(normal, ray.RayDirection)) < EPS)
	{
		return false;
	}

	Vector3f v = TrianglePosition - ray.RayPosition;
	Vector3f crossEdgeB = cross(EdgeB, ray.RayDirection);
	Vector3f crossEdgeA = cross(EdgeA, ray.RayDirection);

	float beta = -dot(crossEdgeB, v) / dot(crossEdgeB, EdgeA);
	float gamma = -dot(crossEdgeA, v) / dot(crossEdgeA, EdgeB);

	float t = dot(normal, v) / dot(normal, ray.RayDirection);

	//inner
	if (beta > 0 && gamma > 0 && beta + gamma < 1 && ray.IsInBounds(t))
	{
		intersection.point = ray.GetPoint(t);
		intersection.normal = getNormal(intersection.point);
		intersection.pBelongTriangle = this;
		ray.UpBound = t;
		return true;
	}

	return false;
}


//-------------------------------- obj model
static bool loadMtlFile(string path, map<string, Material> & material_table)
{
	ifstream file(path);
	if (!file.is_open())
	{
		cerr << path + "open failed!" << endl;
		return false;
	}

	bool flag = false;
	string materialName;
	Material material;
	string type;

	while (file >> type)
	{
		if (type == "newmtl")
		{
			if (flag)
			{
				material_table.insert(make_pair(materialName, material));
				material = Material();
			}
			else 
			{
				flag = true;
			}

			file >> materialName;
			material.name = materialName;
		}
		else if (type == "Kd")//duffuse
		{
			file >> material.Ref.kd.x >> material.Ref.kd.y >> material.Ref.kd.z;
		}
		else if (type == "Ka")//ambient
		{
			file >> material.Ref.ka.x >> material.Ref.ka.y >> material.Ref.ka.z;
		}
		else if (type == "Ks")//specular
		{
			file >> material.Ref.ks.x >> material.Ref.ks.y >> material.Ref.ks.z;
		}
		else if (type == "Ke")//emissive
		{
			file >> material.Emission.r >> material.Emission.g >> material.Emission.b;
		}
		else if (type == "Ns")// shininess
		{
			file >> material.Shiness;
		}
		else if (type == "Ni")//折射系数，材质的光密度（optical density）
		{
			file >> material.RefractiveIndex;
		}
		else if (type == "Tf")//透射滤波（transmission filter）。对应数据为r，g，b，a。
		{
			file >> material.TransmissionFilter.x >> material.TransmissionFilter.y >> material.TransmissionFilter.z;
		}
		else if (type == "illum")//照明度（illumination）
		{
			int illumination;
			file >> illumination;
			material.IsIllumination = true;
		}
		else file.ignore((numeric_limits<streamsize>::max)(), '\n');
	}

	if (flag) material_table.insert(make_pair(materialName, material));//last material

	return true;
}

bool ObjModel::LoadObjFile(const string objFilePath)
{
	ifstream file(objFilePath);
	if (!file.is_open())
	{
		cerr << objFilePath << " open failed! " << endl;
		return false;
	}

	bool isSucceed = true;
	bool isUseMaterial = false;
	bool isUseTexture = false;
	map<string, Material> materialTable;

	string type;
	Point3f v;
	Vector3f vn;
	string materialName;
	Material material;
	int vertexIdx[3];
	int vertexNormalIdx[3];

	while (file >> type)
	{
		//load materialTable
		if (type == "mtllib")
		{
			string mtlFileName;
			file >> mtlFileName;

			int pos = objFilePath.find_last_of('/');
			string mtlPath = objFilePath.substr(0, pos + 1) + mtlFileName;

			if (!loadMtlFile(mtlPath, materialTable))
			{
				//isSucceed = false;
				isUseMaterial = false;
				cerr << mtlPath + "load failed!" << endl;
				//break;
			}
			isUseMaterial = true;
		}
		else if (isUseMaterial && type == "usemtl")//find the material in materialTable 
		{
			file >> materialName;

			map<string, Material>::const_iterator it = materialTable.find(materialName);

			if (it != materialTable.end())
			{
				material = it->second;
			}
			else
			{
				cerr <<materialName + "can not be found in mtl file" << endl;
				isSucceed = false;
				break;
			}
		}
		else if (type == "v")
		{
			file >> v.x >> v.y >> v.z;
			vertices.push_back(v);
		}
		else if (type == "vn")
		{
			file >> vn.x >> vn.y >> vn.z;
			normals.push_back(vn);
		}
		//加载面-顶点索引/纹理索引/法线索引
		else if (type == "f")
		{
			int index = 0;
			memset(vertexIdx, -1, sizeof(vertexIdx));
			memset(vertexNormalIdx, -1, sizeof(vertexNormalIdx));

			while (true)
			{
				char ch = file.get();
				if (ch == ' ') continue;
				else if (ch == '\n' || ch == EOF) break;
				else file.putback(ch);

				file >> vertexIdx[index];//顶点的索引
				--vertexIdx[index];

				char splitter = file.get();

				if (splitter == '/')
				{
					splitter = file.get();
					if (splitter == '/')
					{
						file >> vertexNormalIdx[index];//面法向量的index
						--vertexNormalIdx[index];
					}
					else
					{
						int tIndex;//texture
						file.putback(splitter);
						file >> tIndex;
						splitter = file.get();
						if (splitter == '/')
						{
							file >> vertexNormalIdx[index];
							--vertexNormalIdx[index];
						}
						else file.putback(splitter);
					}
				}
				else file.putback(splitter);

				++index;
				if (index >= 3)
				{
					Triangle* tri = new Triangle(this, vertexIdx, vertexNormalIdx);
					tri->material = material;
					tri->init();
					pTriangles.push_back(tri);

					//add lights
					if (isUseMaterial && material.Emission != BLACK)
					{
#ifndef HIGH
						if (lightCount % LightRate == 0)
						{
							lights.push_back(Light(tri->TrianglePosition, tri->EdgeA, tri->EdgeB, material.Emission * LightRate));
						}
						lightCount++;
#else
						lights.push_back(Light(tri->TrianglePosition, tri->EdgeA, tri->EdgeB, material.Emission));
#endif // LOW
					}
					else if ( material.Ref.ka != BLACK && material.Ref.ka != Color3f(1.0, 1.0, 1.0) )
					{
#ifndef HIGH
						if (lightCount % LightRate == 0)
						{
							lights.push_back(Light(tri->TrianglePosition, tri->EdgeA, tri->EdgeB, material.Ref.ka * LightRate));
						}
						lightCount++;
#else
						lights.push_back( Light(tri->TrianglePosition, tri->EdgeA, tri->EdgeB, material.Ref.ka ) );
#endif // LOW
					}

					vertexIdx[1] = vertexIdx[2];
					vertexNormalIdx[1] = vertexNormalIdx[2];
					index = 2;
				}
			}
		}
	}
	file.close();

	if (!isSucceed)
	{
		cerr << objFilePath + "load failed!" << endl;
	}
	return isSucceed;
}

ObjModel::~ObjModel()
{
	for (int i = 0, len = pTriangles.size(); i < len; ++i)
	{
		if (pTriangles[i] != NULL)
		{
			delete pTriangles[i];
		}
	}
}

//fresnel-schlick
float getFresnelIndex(double ni, double nt, double cosTheta)
{

	float f0 = (ni - nt) / (ni + nt);
	f0 *= f0;
	return  f0 + (1 - f0)* pow(1.0 - cosTheta, 5);
}

Point3f SphereSample(float radius)
{
	double rand1 = (double)rand() / RAND_MAX;
	double rand2 = (double)rand() / RAND_MAX;

	double x = cos(2 * PI * rand2) * 2 * sqrt(rand1 * (rand1 * 2));
	double y = sin(2 * PI * rand2) * 2 * sqrt(rand1 * (rand1 * 2));
	double z = 1 - rand1 * 2;

	return Point3f(radius * x, radius *  y, radius * z);
}

Vector3f HemiSphereSample(Vector3f direction)
{
	double rand1 = (double)rand() / RAND_MAX;
	double rand2 = (double)rand() / RAND_MAX;
	Vector3f front;
	Vector3f right;
	if (fabs(direction.x) > fabs(direction.y))
	{
		front = Vector3f(direction.z, 0, -direction.x);
	}
	else
	{
		front = Vector3f(0, direction.z, -direction.y);
	}
	front = normalize(front);
	right = cross(direction, front);

	double theta = rand1 * 2 * PI;
	double phi = acos(sqrt(rand2));

	//cos(phi) [0,pi/2]
	return normalize(sin(phi) * cos(theta) * right + sin(phi) * sin(theta) * front + cos(phi) * direction);
}

Vector3f HemiSphereImportanceSample(Vector3f direction, float n)
{
	double rand1 = (double)rand() / RAND_MAX;
	double rand2 = (double)rand() / RAND_MAX;
	Vector3f front;
	Vector3f right;

	if (fabs(direction.x) > fabs(direction.y))
	{
		front = Vector3f(direction.z, 0, -direction.x);
	}
	else
	{
		front = Vector3f(0, direction.z, -direction.y);
	}
	front = normalize(front);
	right = cross(direction, front);

	double theta = rand1 * 2 * PI;
	double phi = acos(pow(rand2, 1 / (n * n + 1)));

	//cos(phi) [0,pi/2]
	return normalize(sin(phi) * cos(theta) * right + sin(phi) * sin(theta) * front + cos(phi) * direction);
}

//Vector3f ImportanceSampleGGX(float Roughness, Vector3f N)
//{
//	Vector3f Xi = Vector3f((double)rand() / RAND_MAX, (double)rand() / RAND_MAX, 0);
//	float a = Roughness * Roughness;
//	float Phi = 2 * PI * Xi.x;
//	float CosTheta = sqrt((1 - Xi.y) / (1 + (a*a - 1) * Xi.y));
//	float SinTheta = sqrt(1 - CosTheta * CosTheta);
//	Vector3f H;
//	H.x = SinTheta * cos(Phi);
//	H.y = SinTheta * sin(Phi);
//	H.z = CosTheta;
//	Vector3f UpVector = abs(N.z) < 0.999 ? Vector3f(0, 0, 1) : Vector3f(1, 0, 0);
//	Vector3f TangentX = normalize(cross(UpVector, N));
//	Vector3f TangentY = cross(N, TangentX);
//	// Tangent to world space
//	return TangentX * H.x + TangentY * H.y + N * H.z;
//}

// smaller probability more true
bool RussianRouletteSample(float probability)
{
	return ((double)rand() / RAND_MAX) > probability;
}

Ray MonteCarloSample(Ray & ray, Intersection & intersection)
{
	Material & material = intersection.pBelongTriangle->material;
	Reflectance & ref = intersection.pBelongTriangle->material.Ref;

	Vector3f direction;

	float kd = dot(ref.kd, Vector3f(1, 1, 1));
	float kskd = dot(ref.ks, Vector3f(1, 1, 1)) + kd;

	if (kskd <= 0) //none type light
	{
		return Ray(intersection.point, direction);
	}

	if (material.IsIllumination && material.RefractiveIndex != 1.0)
	{
		float ni, nt;
		float cosValue = dot(ray.RayDirection, intersection.normal);

		Vector3f normal;
		if (cosValue > 0.0)
		{
			normal = -intersection.normal;
			ni = material.RefractiveIndex;
			nt = 1.0;
		}
		else
		{
			normal = intersection.normal;
			ni = 1.0;
			nt = material.RefractiveIndex;
		}


		//Reflection or refraction
		if ( RussianRouletteSample( getFresnelIndex(ni, nt, abs(cosValue) ) ) )
		{
			if (ray.GetIsRefractDirection(normal, ni / nt, direction))
			{
				return Ray(intersection.point, direction, LightSourceType::TRANSMISSON);
			}
			else
			{
				direction = ray.GetReflect(normal);
				return Ray(intersection.point, direction, LightSourceType::SPECULAR_REFLECT);
			}
		}
	}

	if ( RussianRouletteSample( kd / kskd ) )
	{
		Vector3f ReflectDirection = ray.GetReflect(intersection.normal);
		direction =  HemiSphereImportanceSample(ReflectDirection, material.Shiness) ;
		//// makesure direction above up hemiSphere
		//if ( dot(intersection.normal, direction) < 0)
		//{
		//	direction =  HemiSphereImportanceSample(ReflectDirection, material.Shiness);
		//}
		return Ray(intersection.point, direction, LightSourceType::SPECULAR_REFLECT);
	}
	else
	{
		direction = HemiSphereImportanceSample(intersection.normal, 1);// it is not totalReflectDirection
		return Ray(intersection.point, direction, LightSourceType::DIFFUSE_REFLECT);
	}
}