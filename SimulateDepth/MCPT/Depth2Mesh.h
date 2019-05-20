#pragma once

// C++ 标准库
#include <iostream>
#include <fstream>
#include <string>


// OpenCV 库
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "common.h"
#include "ObjFileHandle.h"

#define gydebug
//#define gydebugDirectionXYZ



class DepthHandle
{
public:
	DepthHandle();
	DepthHandle(std::string depthImageName, std::string outCloudName, std::string configFileName = "depth2cloudconfig.txt" );
	// float map to do DepthHandle(); //need not configFileName
	DepthHandle(double * depthmap, int _width, int _height, std::string outCloudName, std::string configFileName = "depth2cloudconfig.txt");
	~DepthHandle();

	int BilateralFilter();

	int ReadConfig(std::string configFileName);
	int ReadDepthImage(std::string depthImageName);
	int Depth2cloud(std::string outCloudName);
	int Depth2cloudNet(std::string outCloudName);

	int ReadConfig() { return ReadConfig(configName); }
	int ReadDepthImage() { return ReadDepthImage(depthName); }
	int Depth2cloud() { return Depth2cloud(outFileName); }
	int Depth2cloudNet() { return Depth2cloudNet(outFileName); }

	int AccurateDepth2cloudNet();

private:

	bool isFacePoint(int r,int c);
	int width, height;
	double * depthmap;
	cv::Mat depthMat;
	std::vector< std::vector<IDPoint> > pointMat;
	std::vector<IDPoint> vertices;
	std::vector<IDQuadrangle> quadrangles;
	std::vector<IDTriangle> triangles;
	// 相机内参

	// my demo
	double camera_factor = 1000;
	double camera_cx = 512.0;
	double camera_cy = 512.0;
	double camera_fx = 1024.0;
	double camera_fy = 1024.0;
	double MinDepth = 0;
	double MaxDepth = 300;
	double CutRate = 0;

	//double maxVisiableAngle = 60;
	double minCosVisiableAngle = 0.5;

	int g_ndValue = 10;
	int g_nsigmaColorValue = 10;
	int g_nsigmaSpaceValue = 10;

	/*
	// my kinect
	const double camera_factor = 1000;
	const double camera_cx = 256.0;
	const double camera_cy = 212.0;
	const double camera_fx = 512.0;
	const double camera_fy = 424.0;
	*/

	//kinect
	// double camera_factor = 1000;
	// double camera_cx = 325.5;
	// double camera_cy = 253.5;
	// double camera_fx = 518.0;
	// double camera_fy = 519.0;

	// double MinDepth = 10;
	// double MaxDepth = 30;
	// double CutRate = 0.15;

	std::string configName;
	std::string depthName;
	std::string outFileName;
};
