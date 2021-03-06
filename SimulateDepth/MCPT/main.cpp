// MCPT.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "pch.h"
#include "Display.h"

//#include "Depth2Mesh.h"
#define gydebug

using namespace std;

void scene1cup(string scenesName)
{
	Scene scene(scenesName,100);
	//scene.pCamera.LookAt(0, 0.64, 0.52, 0, 0.4, 0.3, 0, 1, 0); // M 1 -0 0 0 0.675725 -0.737154 0 -0.737154 -0.675725

	scene.pCamera.LookAt(0, 0.64, 0.52, 0, 0, 0); //M 1 -0 0 0 -0.630593 0.776114 0 -0.776114 -0.630593
	//scene.pCamera.LookAt(0.52, 0.64, 0, 0, 0, 0, 0, 1, 0); //M -0 0 -1 0.776114 -0.630593 0 -0.630593 -0.776114 0
	//scene.pCamera.LookAt(0.26 * sqrtf(2), 0.64, -0.26 * sqrtf(2), 0, 0, 0, 0, 1, 0);//M -0.707107 0 -0.707107 0.548796 -0.630593 -0.548796 -0.445896 -0.776114 0.445896 	P 36.7696 64 - 36.7696
	//scene.pCamera.LookAt(-0.26 * sqrtf(2), 0.64, -0.26 * sqrtf(2), 0, 0, 0, 0, 1, 0);//M -0.707107 0 0.707107 -0.548796 -0.630593 -0.548796 0.445896 -0.776114 0.445896 	P - 36.7696 64 - 36.7696


	scene.pCamera.CalcViewPort(60, 1024, 1024);
	scene.lights = scene.inputModel.lights;
	//scene.lights.push_back(Light(Point3f(-2.758771896, 1.5246, 0), Vector3f(0, 0, -1), Vector3f(0, 1, 0), Vector3f(1, 0, 0), Color3f(40, 40, 40)));
	scene.SceneInit("cup");

	Display display(&scene);
	display.RunDoubleWindows();
}

void scene2room(string scenesName= "scenes/Scene02/room.obj")
{
	Scene scene(scenesName, 10);
	scene.pCamera.LookAt(0, 0, 4, 0, 0, 0, 0, 1, 0);//

	scene.pCamera.CalcViewPort(50, 512, 512);
	scene.lights.push_back(Light(Point3f(0.0, 1.589, -1.274), 0.2, Color3f(50, 50, 40)));
	scene.SceneInit("scene2room");


	//PathTracer rayTracer(&scene, 0.1);
	Display display(&scene);
	display.RunDoubleWindows();
}

void scene3VeachMIS(string scenesName= "scenes/Scene03/VeachMIS.obj")
{
	Scene scene(scenesName, 100);
	scene.pCamera.LookAt(0, 2, 15, 0, 1.69521, 14.0476, 0, 0.952421, -0.304787);
	// 28 1152 864
#ifdef LOW
	scene.pCamera.CalcViewPort(40, 512, 512);
#else
	scene.pCamera.CalcViewPort(38, 1152, 864);
#endif // LOW
	scene.lights.push_back(Light(Point3f(-10, 10, 4), 0.5, Color3f(800, 800, 800)));
	scene.lights.push_back(Light(Point3f(3.75, 0, 0), 0.033, Color3f(901.803, 901.803, 901.803)));
	scene.lights.push_back(Light(Point3f(1.25, 0, 0), 0.1, Color3f(100, 100, 100)));
	scene.lights.push_back(Light(Point3f(-1.25, 0, 0), 0.3, Color3f(11.1111, 11.1111, 11.1111)));
	scene.lights.push_back(Light(Point3f(-3.75, 0, 0), 0.9, Color3f(1.23457, 1.23457, 1.23457)));
	//scene.lights = model.lights;
	scene.SceneInit("scene3VeachMIS");

	//PathTracer rayTracer(&scene, 0.5);
	Display display(&scene);
	display.RunDoubleWindows();
}

void obj2obj()
{
	double * depthmap = new double[1024 * 1024];
	Scene scene("scenes/Scene01/cup.obj", 100);
	scene.pCamera.LookAt(0, 0.64, 0.52, 0, 0, 0);
	scene.pCamera.CalcViewPort(60, 1024, 1024);
	scene.lights = scene.inputModel.lights;
	scene.SceneInit("cup");
	Display display(&scene);
	display.RunDoubleWindows();

	//DepthHandle * adepth = new DepthHandle(depthmap, "scenes/output_accurate.obj", "scenes/depth2cloudconfig.txt");
	////adepth->AccurateDepth2cloudNet();
}
int main()
{
	char inOption;
	DepthHandle *adepth;
	//cin >> inOption;
	inOption = '3';
	
	switch (inOption) {
	case '1':
		scene1cup("scenes/Scene01/cup.obj");
		break;
	case '2':
		adepth=new DepthHandle("scenes/cup_0.png","scenes/output.obj","scenes/depth2cloudconfig.txt");
		adepth->Depth2cloudNet();
		cin >> inOption;
		break;
	case '3':
		obj2obj();
		break;
	default:
		break;
	}

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
