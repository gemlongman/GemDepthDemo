#include "ObjFileHandle.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

int ObjFileHandle::SaveFile(string fileName)
{
	ofstream file(fileName);
	if (!file.is_open())
	{
		cerr << fileName + "open failed!" << endl;
		return -1;
	}

	vector<IDPoint>::iterator itVertices = vertices->begin();
	while (vertices->end() != itVertices )
	{
		file << "v " << itVertices->point.x << " " << itVertices->point.y << " " << itVertices->point.z << endl;
		itVertices++;
	}

	if (trirangles)
	{
		vector<IDTriangle>::iterator itIDTriangle = trirangles->begin();
		while (trirangles->end() != itIDTriangle)
		{
			file << "f " << itIDTriangle->point[0] << " " << itIDTriangle->point[1] << " " << itIDTriangle->point[2] << endl;
			itIDTriangle++;
		}
	}


	if (quadrangles)
	{
		vector<IDQuadrangle>::iterator itIDQuadrangle = quadrangles->begin();
		while (quadrangles->end() != itIDQuadrangle)
		{
			//0 1
			//2 3
			file << "f " << itIDQuadrangle->point[0] << " " << itIDQuadrangle->point[3] << " " << itIDQuadrangle->point[1] << endl;
			file << "f " << itIDQuadrangle->point[0] << " " << itIDQuadrangle->point[2] << " " << itIDQuadrangle->point[3] << endl;
			itIDQuadrangle++;
		}
	}


	file.close();
	return 0;
}