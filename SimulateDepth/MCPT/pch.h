// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

#ifndef PCH_H
#define PCH_H

#include <cmath>
#include <io.h>
#include <string>
#include <vector>
#include <iostream>
#include <climits>
#include <algorithm>

#define LOW

#ifdef LOW
#define PixSampleNum 1//1 1 3
#define LightSampleNum 1
#define MaxRescursiveDepth 2//3 5 8
#define LightRate 8
#define MaxIterationsNum 2
#endif

#ifdef NORMAL
#define PixSampleNum 1
#define LightSampleNum 3
#define MaxRescursiveDepth 5
#define LightRate 4
#define MaxIterationsNum 32
#endif 

#ifdef HIGH
#define PixSampleNum 3
#define LightSampleNum 10
#define MaxRescursiveDepth 5
#define LightRate 1
#define MaxIterationsNum 128
#endif 

const float EPS = 1e-5f;
const float PI = 3.1415926f;
bool isEqualf(float a, float b);
#endif
