#pragma once
#include "common.h"

class ObjFileHandle {
public:
	ObjFileHandle(std::vector<IDPoint> & _vertices, std::vector<IDQuadrangle> & _quadrangles) 
	{
		vertices = &_vertices;
		quadrangles = &_quadrangles;
	};
	ObjFileHandle(std::vector<IDPoint> & _vertices, std::vector<IDTriangle> & _trirangles)
	{
		vertices = &_vertices;
		trirangles = &_trirangles;
	};
	int SaveFile(std::string fileName);
private:
	std::string fileName;
	std::vector<IDPoint> * vertices;
	std::vector<IDQuadrangle> * quadrangles;
	std::vector<IDTriangle> * trirangles;
	

};