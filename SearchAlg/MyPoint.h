#ifndef MYPOINT_H
#define MYPOINT_H

#include <atltypes.h>
using namespace std;
struct MyPoint{
	MyPoint(){//默认构造函数
		x = 0.0;
		y = 0.0;
	}
	MyPoint(double x1, double y1){//构造函数
		x = x1;
		y = y1;
	}
	double x;
	double y;
	CPoint CvtToCP_Grid(double xRatio, double yRatio, int xPadding, int yPadding);//栅形搜索：真实坐标转换为屏幕坐标
	CPoint CvtToCP_Square(double xRatio, double yRatio, int xPadding, int yPadding, int centerX, int centerY);//方形搜索：真实坐标转换为屏幕坐标

};
#endif