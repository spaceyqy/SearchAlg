#include "stdafx.h"
#include "MyPoint.h"

//栅形搜索：真实坐标转换为屏幕坐标
CPoint MyPoint::CvtToCP_Grid(double xRatio, double yRatio, int xPadding, int yPadding)
{
	return CPoint(int(xRatio*x + xPadding), int(yRatio*y + yPadding));
}

//方形搜索：真实坐标转换为屏幕坐标
CPoint MyPoint::CvtToCP_Square(double xRatio, double yRatio, int xPadding, int yPadding,int centerX,int centerY)
{
	//以centerX和centerY为坐标系原点
	return CPoint(int(centerX+x*xRatio) , int(centerY-y*yRatio));
}