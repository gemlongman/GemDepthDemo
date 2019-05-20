#pragma once
#define  _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996);
#include "PathTracer.h"
#include "DepthTracer.h"

#include "Depth2Mesh.h"

#include <time.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include<opencv2\opencv.hpp>   
#include<opencv2\highgui\highgui.hpp>
#include "glut/glut.h"
//using namespace std;

class Display
{
public:
	Display(Scene* _pScene);
	Display(Scene* _pScene, float _ambient);
	~Display() 
	{
		//delete thisPathTracer;
		delete thisDepthTracer;
	}
	void Run2DWindow();
	void RunGl3DWindow();
	void RunDoubleWindows();

	
private:
	static double * pixDepthAccuate;
	//static PathTracer * thisPathTracer;
	static DepthTracer * thisDepthTracer;
	static Scene * thisScene;
	static Camera * thisCamera;
	static int caputerNumber, mainWindow, subWindowLeft, subWindowRight;
	static double cameraFactor;

	static void RenderMainWindow();
	static void Render2D();
	static void RenderGl3D();

	static void Update();
	static void ScreenShot(int width, int height, const char * filepath);
	static void DepthShot(unsigned short * pData, int iWidth, int iHeight, const char * filepath);
	static void KeyboardHandle(unsigned char key, int x, int y);
	static void KeyboardSpecialHandle(int key, int x, int y);
	//static void AutoScreenShot();
	static void WritetxtImage(unsigned short  * imgbuf, int width, int height, const char * filepath);
	static void WriteImage(unsigned short  * imgbuf, int width, int height, const char * filepath);

	static void reshape(int w, int h);
	static void changeSize(int w1, int h1); // todo
	static void setProjection(int w1, int h1);

};


Camera * Display::thisCamera = NULL;
Scene * Display::thisScene = NULL;
//PathTracer * Display::thisPathTracer = NULL;
DepthTracer * Display::thisDepthTracer = NULL;

double * Display::pixDepthAccuate=NULL;

int Display::caputerNumber = 0;
int Display::mainWindow = 0;
int Display::subWindowLeft = 0;
int Display::subWindowRight = 0;
double Display::cameraFactor=0.0;

Display::Display(Scene* _pScene)
{
	caputerNumber = 0;
	thisScene = _pScene;
	thisCamera = &_pScene->pCamera;
	cameraFactor = _pScene->pCamera.CameraFactor;
	thisDepthTracer = new DepthTracer(_pScene);
	pixDepthAccuate = new double[thisScene->GetWidth()*thisScene->GetHeight()];
}

Display::Display(Scene* _pScene, float _ambient)
{
	caputerNumber = 0;
	thisScene = _pScene;
	thisCamera = & _pScene->pCamera;
	cameraFactor = _pScene->pCamera.CameraFactor;
	//thisPathTracer = new PathTracer(_pScene, _ambient);
	thisDepthTracer = new DepthTracer(_pScene);
}

void Display::Update()
{
	//glutPostRedisplay();
	RenderGl3D();
	Render2D();
}

void Display::Render2D()
{
	glutSetWindow(subWindowRight);

	int width = thisScene->GetWidth();
	int height = thisScene->GetHeight();

	//glClear(GL_DEPTH_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);

	//glRasterPos2i(0, 0);
	//float* colors = thisPathTracer->PathTracerRender();
	//glDrawPixels(width, height, GL_RGB, GL_FLOAT, (GLvoid *)colors);

	glRasterPos2i(0, 0);
	unsigned short* depths = thisDepthTracer->DepthTracerScanner(cameraFactor);
	glDrawPixels(width, height, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (GLvoid *)depths);

	glutSwapBuffers();
	//glFlush();
}


void Display::RenderGl3D()
{
	glutSetWindow(subWindowLeft);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glViewport(0, 0, (GLsizei)thisScene->GetWidth(), (GLsizei)thisScene->GetHeight());
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(thisCamera->fov, (GLfloat)thisScene->GetWidth() / (GLfloat)thisScene->GetHeight(),0.01, 1000.0);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(
		thisCamera->CameraPosition.x, thisCamera->CameraPosition.y, thisCamera->CameraPosition.z,
		thisCamera->LookAtPosition.x, thisCamera->LookAtPosition.y, thisCamera->LookAtPosition.z,
		//0,1,0);
		thisCamera->up.x, thisCamera->up.y, thisCamera->up.z);

	//glColorMaterial(GL_FRONT,GL_DIFFUSE);
	//glClearColor(1, 1, 1, 0);

	if (thisDepthTracer->GetScene()->lights.size()>0) {
		GLfloat light_position[] = {thisScene->lights[0].LightPosition.x,thisDepthTracer->GetScene()->lights[0].LightPosition.y,thisDepthTracer->GetScene()->lights[0].LightPosition.z,0 };
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	}
	else
	{
		GLfloat light_position[] = { 1,1,1,0 };
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	}
	GLfloat light_diffuse[] = { 0.5,0.5,0.5,1 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	GLfloat mat_emission[] = { 0 , 0.2, 0.5, 0.0 }; //决定了三为物体表面颜色 
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	static ObjModel & allmodel = thisScene->inputModel;
	
	for (int i = 0; i < allmodel.pTriangles.size(); i++)
	{
		Triangle * aTriangle = allmodel.pTriangles[i]->GeTriangle();

		if (aTriangle)
		{
			glBegin(GL_TRIANGLES);//GL_TRIANGLES	GL_LINE_LOOP	GL_LINES	GL_POINTS

			auto & Ref = aTriangle->material.Ref;
			if (Ref.kd !=BLACK)
			{
				GLfloat mat_emission[] = { Ref.kd.r , Ref.kd.g, Ref.kd.b, 0.0 };
				glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
			}
			if (Ref.ka != BLACK)
			{
				GLfloat mat_ambient[] = { Ref.ka.r , Ref.ka.g, Ref.ka.b, 0.0 };
				//glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
			}
			if (Ref.ks != BLACK)
			{
				GLfloat mat_specular[] = { Ref.ks.r , Ref.ks.g, Ref.ks.b, 0.0 };
				//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
			}

			// opengl是左手坐标系，所以画面片和三角型都是 -x
			glNormal3f( aTriangle->normal.x, aTriangle->normal.y, aTriangle->normal.z );// it is better to use face normal
			
			glVertex3f( allmodel.vertices[ aTriangle->VertexIdx[0] ].x , allmodel.vertices[aTriangle->VertexIdx[0]].y , allmodel.vertices[aTriangle->VertexIdx[0]].z );

			glVertex3f(allmodel.vertices[aTriangle->VertexIdx[1]].x, allmodel.vertices[aTriangle->VertexIdx[1]].y, allmodel.vertices[aTriangle->VertexIdx[1]].z);

			glVertex3f(allmodel.vertices[aTriangle->VertexIdx[2]].x, allmodel.vertices[aTriangle->VertexIdx[2]].y, allmodel.vertices[aTriangle->VertexIdx[2]].z);
			glEnd();
		}

		//glBegin(GL_POINTS);                            // 绘制三角形GL_TRIANGLES;GL_LINE_LOOP;GL_LINES;GL_POINTS
		//if (m_pic.VT.size() != 0)glTexCoord2f(m_pic.VT[m_pic.F[i].T[0]].TU, m_pic.VT[m_pic.F[i].T[0]].TV);  //纹理    
		//if (m_pic.VN.size() != 0)glNormal3f(m_pic.VN[m_pic.F[i].N[0]].NX, m_pic.VN[m_pic.F[i].N[0]].NY, m_pic.VN[m_pic.F[i].N[0]].NZ);//法向量
		//glVertex3f(m_pic.V[m_pic.F[i].V[0]].X / YU, m_pic.V[m_pic.F[i].V[0]].Y / YU, m_pic.V[m_pic.F[i].V[0]].Z / YU);        // 上顶点

		//if (m_pic.VT.size() != 0)glTexCoord2f(m_pic.VT[m_pic.F[i].T[1]].TU, m_pic.VT[m_pic.F[i].T[1]].TV);  //纹理
		//if (m_pic.VN.size() != 0)glNormal3f(m_pic.VN[m_pic.F[i].N[1]].NX, m_pic.VN[m_pic.F[i].N[1]].NY, m_pic.VN[m_pic.F[i].N[1]].NZ);//法向量
		//glVertex3f(m_pic.V[m_pic.F[i].V[1]].X / YU, m_pic.V[m_pic.F[i].V[1]].Y / YU, m_pic.V[m_pic.F[i].V[1]].Z / YU);        // 左下

		//if (m_pic.VT.size() != 0)glTexCoord2f(m_pic.VT[m_pic.F[i].T[2]].TU, m_pic.VT[m_pic.F[i].T[2]].TV);  //纹理
		//if (m_pic.VN.size() != 0)glNormal3f(m_pic.VN[m_pic.F[i].N[2]].NX, m_pic.VN[m_pic.F[i].N[2]].NY, m_pic.VN[m_pic.F[i].N[2]].NZ);//法向量
		//glVertex3f(m_pic.V[m_pic.F[i].V[2]].X / YU, m_pic.V[m_pic.F[i].V[2]].Y / YU, m_pic.V[m_pic.F[i].V[2]].Z / YU);        // 右下
		//glEnd();// 三角形绘制结束    


	}
	glutSwapBuffers();
	//glFlush();
}


void  Display::WriteImage(unsigned short  * imgbuf, int width, int height, const char * filepath)
{
	//cv::Mat rows,   cols,  type
	cv::Mat dst(height, width,0);

	for (int y = 0; y < height; y++)
	//for (int y = height -1; y > -1; y--)
	{
		for (int x = 0; x < width; x++)
		{
			dst.ptr<uchar>(height-1-y)[x] = imgbuf[y * width + x];
		}

	}
	//vector<int> compression_params;
	//compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION); //PNG格式图片的压缩级别  
	//compression_params.push_back(9);  //这里设置保存的图像质量级别

	cv::imwrite(filepath, dst); //保存处理后的图片;

#ifdef gydebug
	cv::imshow("imshow", dst);
#endif // gydebug

	
}

void  Display::WritetxtImage(unsigned short  * imgbuf, int width, int height, const char * filepath)
{
	ofstream imgFo(filepath);
	if (!imgFo)
	{
		cerr << "open error!" << endl;
		abort();
	}
	imgFo << "M";
	//auto cam =thisScene->pCamera;
	imgFo << " " << thisCamera->right.x << " " << thisCamera->right.y << " " << thisCamera->right.z;//X
	imgFo << " " << -thisCamera->up.x << " " << -thisCamera->up.y << " " << -thisCamera->up.z;//-Y
	imgFo << " " << thisCamera->CameraDirection.x << " " << thisCamera->CameraDirection.y << " " << thisCamera->CameraDirection.z << endl;//Z
	imgFo << "P" << " " << thisCamera->CameraPosition.x * (cameraFactor) << " " << thisCamera->CameraPosition.y * (cameraFactor) << " " << thisCamera->CameraPosition.z * (cameraFactor) << endl;//position

	//imgFo << "R x " << endl;

	////仰角 R x 30
	//Vector3f front(thisCamera->CameraDirection.x, 0, thisCamera->CameraDirection.z);
	//front = normalize(front);
	//float angle = acosf( dot(front, thisCamera->CameraDirection) );
	//imgFo << "R x " << angle << endl;

	////转角 R y 30
	//angle = acosf( dot( front, Vector3f(0,0,-1) ) );
	//imgFo << "R y " << angle << endl;

	imgFo.close();

}

void Display::ScreenShot(int width, int height,const char * filepath)
{
	GLint pView[4];
	glGetIntegerv(GL_VIEWPORT, pView);//得到视图矩阵,pView[2]为宽即width,pView[3]为高即height

	GLsizei numComponet = 3;
	GLsizei bufferSize = pView[2] * pView[3] * sizeof(GLfloat)*numComponet;

	GLfloat * _data = new GLfloat[bufferSize];
	unsigned char *  data = new unsigned char[bufferSize];
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);//设置4字节对齐
	glReadBuffer(GL_FRONT);
	glReadPixels(pView[0], pView[1], pView[2], pView[3], GL_BGR_EXT, GL_FLOAT, _data);//不是GL_RGB的读取方式，而是GL_BGR或者GL_BGR_Ext
	glReadBuffer(GL_BACK);
	for (int i = 0; i < bufferSize; i++)
	{
		data[i] = _data[i] * 255;
	}
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	infoHeader.biSize = 40;
	infoHeader.biWidth = width;
	infoHeader.biHeight = height;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 24;
	infoHeader.biCompression = BI_RGB;
	infoHeader.biSizeImage = pView[2] * pView[3] * 3;
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;
	fileHeader.bfType = 0x4D42;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = 54;
	fileHeader.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pView[2] * pView[3] * 3);
	FILE *fd;
	if (!(fd = fopen(filepath, "wb+")))
	{
		cout<<"bmp fopen error";
		//exit(0);
	}
	else
	{
		fwrite(&fileHeader, 1, sizeof(BITMAPFILEHEADER), fd);
		fwrite(&infoHeader, 1, sizeof(BITMAPINFOHEADER), fd);
		fwrite(data, 1, pView[2] * pView[3] * 3, fd);
		fclose(fd);
	}
	delete[] data;
	delete[] _data;
}

void Display::DepthShot(unsigned short * pData, int iWidth, int iHeight, const char * filepath)
{
	BITMAPFILEHEADER targetfileheader;
	BITMAPINFOHEADER targetinfoheader;
	memset(&targetfileheader, 0, sizeof(BITMAPFILEHEADER));
	memset(&targetinfoheader, 0, sizeof(BITMAPINFOHEADER));

	targetfileheader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
	targetinfoheader.biSize = iWidth * iHeight + sizeof(RGBQUAD) * 256 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	targetfileheader.bfReserved1 = 0;
	targetfileheader.bfReserved2 = 0;
	targetfileheader.bfType = 0x4D42;

	targetinfoheader.biBitCount = 8;
	targetinfoheader.biSize = sizeof(BITMAPINFOHEADER);
	targetinfoheader.biHeight = iHeight;
	targetinfoheader.biWidth = iWidth;
	targetinfoheader.biPlanes = 1;
	targetinfoheader.biCompression = BI_RGB;
	targetinfoheader.biSizeImage = 0;
	targetinfoheader.biXPelsPerMeter = 0;
	targetinfoheader.biYPelsPerMeter = 0;
	targetinfoheader.biClrImportant = 0;
	targetinfoheader.biClrUsed = 0;

	RGBQUAD rgbquad[256];
	int i;
	for (i = 0; i < 256; i++)
	{
		rgbquad[i].rgbBlue = i;
		rgbquad[i].rgbGreen = i;
		rgbquad[i].rgbRed = i;
		rgbquad[i].rgbReserved = 0;
	}


	FILE * fp = fopen(filepath, "wb");
	fwrite(&targetfileheader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&targetinfoheader, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(&rgbquad, sizeof(RGBQUAD), 256, fp);

	for (i = 0; i < iHeight; i++)
	//for (i = iHeight-1; i > -1; i--)
	{
		//fwrite(pData + (iHeight - 1 - i) * iWidth, iWidth, 1, fp);
		for (int j = 0; j < iWidth; j++)
		{
			unsigned char pixVal = pData[i*iWidth + j];
			fwrite(&pixVal, sizeof(unsigned char), 1, fp);
		}
	}
	fclose(fp);

}

void Display::KeyboardHandle(unsigned char key, int x, int y)
{
	string tfilename = "scenes\\" +thisScene->SceneName;
	char timenow[20];
	DepthHandle * adepth;
	auto tis = clock();
	switch (key) {
	case 'c':
	case 'C':
		//sprintf(timenow, "%d", clock());
		//tfilename += timenow;
		sprintf(timenow, "_%d", caputerNumber);
		tfilename += timenow;
		tfilename += "_compare.bmp";
		ScreenShot(thisScene->GetWidth()*2, thisScene->GetHeight(), tfilename.c_str());
		cout << "save capture bmp：" << tfilename << endl;
		break;
	case 'd':
	case 'D':
		sprintf(timenow, "_%d", caputerNumber++);
		tfilename += timenow;
		thisDepthTracer->DepthTracerScanner(cameraFactor, pixDepthAccuate);

		WriteImage(thisDepthTracer->GetScene()->pixDepth,thisScene->GetWidth(),thisScene->GetHeight(), (tfilename+".png").c_str());
		WritetxtImage(thisDepthTracer->GetScene()->pixDepth,thisScene->GetWidth(),thisScene->GetHeight(), (tfilename + "_Matrix.txt").c_str() );
		//DepthShot(thisDepthTracer->GetScene()->pixDepth,thisScene->GetWidth(),thisScene->GetHeight(), (tfilename+".bmp").c_str());
		cout << "save depth bmp：" << tfilename << endl << clock() << endl;
		break;
	case '=':
	case '+':
		thisCamera->MoveForward(1);
		break;
	case '-':
		thisCamera->MoveForward(-1);
		break;
	case 's':
		tis = clock();
		cout <<"T " <<CLOCKS_PER_SEC << endl;
		thisDepthTracer->DepthTracerScanner(cameraFactor, pixDepthAccuate);

		adepth = new DepthHandle(pixDepthAccuate, thisScene->GetWidth(), thisScene->GetHeight(), "scenes/output_accurate.obj", "scenes/depth2cloudconfig.txt");
		delete(adepth);
		adepth = NULL;
		cout << "time " << clock() - tis << endl;
		break;
	default:
		break;
	}
}

void Display::KeyboardSpecialHandle(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
		//cout << "L" << endl; 
		thisCamera->Turn(1);
		break;
	case GLUT_KEY_RIGHT:
		//cout << "R" << endl;
		thisCamera->Turn(-1);
		break;
	case GLUT_KEY_UP:
		//cout << "up" << endl;
		thisCamera->Pitch(1);
		break;
	case GLUT_KEY_DOWN:
		//cout << "down" << endl;
		thisCamera->Pitch(-1);
		break;
	case GLUT_KEY_PAGE_UP:
		//cout << "page up" << endl;
		thisCamera->MoveForward(1);
		break;
	case GLUT_KEY_PAGE_DOWN:
		//cout << "page down" << endl;
		thisCamera->MoveForward(-1);
		break;
	}
}


void Display::reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(thisCamera->fov, (GLfloat)w / (GLfloat)h, 0.01, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(thisCamera->CameraPosition.x, thisCamera->CameraPosition.y, thisCamera->CameraPosition.z,
		thisCamera->LookAtPosition.x, thisCamera->LookAtPosition.y, thisCamera->LookAtPosition.z,
		thisCamera->up.x, thisCamera->up.y, thisCamera->up.z);
}


void Display::RunGl3DWindow()
{
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(thisScene->GetWidth(), thisScene->GetHeight() );
	glutInitWindowPosition(0, 0);
	glutCreateWindow("test");
	
	glClearColor(0.5, 0.5, 0.5, 0);
	GLfloat light_position[] = { 1,1,1,0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	GLfloat light_diffuse[] = { 0.5,0.5,0.5,1 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	GLfloat mat_emission[] = {0 , 1, 1, 0.0 }; //决定了三为物体表面颜色 
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

	//glColorMaterial(GL_FRONT,GL_DIFFUSE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(RenderGl3D);
	glutReshapeFunc(reshape);

	//glutMotionFunc(motion);
	//glutMouseFunc(mouse);
	glutKeyboardFunc(KeyboardHandle);
	glutMainLoop();

}

void Display::Run2DWindow()
{
	int width = thisScene->GetWidth();
	int height = thisScene->GetHeight();

	cout << endl << endl << "pixs: " << width << " * " << height << endl << "factor: " << (cameraFactor) << endl << endl;

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("CGMCPT");

	glutDisplayFunc(Render2D);

	//glutIdleFunc(Update);
	glutKeyboardFunc(KeyboardHandle);
	glutMainLoop();
}

void Display::setProjection(int w1, int h1)
{

	float ratio;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	ratio = 1.0f * w1 / h1;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w1, h1);


	// Set the clipping volume
	gluPerspective(thisCamera->fov, ratio, 0.01, 1000);
	glMatrixMode(GL_MODELVIEW);


	//glViewport(0, 0, (GLsizei)w1, (GLsizei)h1);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(thisCamera->fov,(GLfloat)w1 / (GLfloat)h1 , 0, 1000.0);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//gluLookAt(
	//	thisCamera->CameraPosition.x, thisCamera->CameraPosition.y, thisCamera->CameraPosition.z,
	//	thisCamera->LookAtPosition.x, thisCamera->LookAtPosition.y, thisCamera->LookAtPosition.z,
	//	thisCamera->up.x, thisCamera->up.y, thisCamera->up.z);
}

void Display::changeSize(int w1, int h1) {

	if (h1 == 0)
		h1 = 1;

	// we're keeping these values cause we'll need them latter
	int w = w1;
	int h = h1;

	// set subwindow 1 as the active window
	glutSetWindow(subWindowLeft);
	// resize and reposition the sub window
	glutPositionWindow(0, 0);
	glutReshapeWindow(w / 2, h);
	setProjection(w / 2, h);

	// set subwindow 2 as the active window
	glutSetWindow(subWindowRight);
	// resize and reposition the sub window
	glutPositionWindow(w / 2, 0);
	//glutReshapeWindow(w / 2, h );
	//setProjection(w / 2 , h );

}

// Display func for main window
void Display::RenderMainWindow() {
	glutSetWindow(mainWindow);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}


void Display::RunDoubleWindows()
{
	int width = thisScene->GetWidth();
	int height = thisScene->GetHeight();

	cout << endl << endl << "pixs: " << width << " * " << height << endl << "factor: " << (cameraFactor) << endl << endl;

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width * 2, height);
	glutInitWindowPosition(0, 0);
	mainWindow  = glutCreateWindow("CGMCPT");

	// callbacks for main window
	glutDisplayFunc(RenderMainWindow);
	glutReshapeFunc(changeSize); // http://www.lighthouse3d.com/tutorials/glut-tutorial/subwindow-reshape/
	glutIdleFunc(Update);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutKeyboardFunc(KeyboardHandle);
	glutSpecialFunc(KeyboardSpecialHandle);

	// sub windows
	subWindowLeft = glutCreateSubWindow(mainWindow, 0, 0, width, height);
	glutDisplayFunc(RenderGl3D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	subWindowRight = glutCreateSubWindow(mainWindow, width, 0, width, height);
	glutDisplayFunc(Render2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	glutMainLoop();


}