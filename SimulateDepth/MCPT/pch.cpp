// pch.cpp: 与预编译标头对应的源文件；编译成功所必需的

#include "pch.h"


bool isEqualf(float a, float b)
{
	if (fabs(a - b) < EPS)
	{
		return true;
	}
	return false;

}
