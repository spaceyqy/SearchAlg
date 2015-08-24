
// SearchAlg.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "gdiplus.h"//引入GDI+头文件

using namespace Gdiplus;//使用GDI+命名空间
#pragma comment(lib, "gdiplus.lib")

// CSearchAlgApp: 
// 有关此类的实现，请参阅 SearchAlg.cpp
//

class CSearchAlgApp : public CWinApp
{
public:
	CSearchAlgApp();//构造函数

// 重写
public:
	virtual BOOL InitInstance();
	ULONG_PTR m_gdiplusToken;//声明一个GDI+变量
	GdiplusStartupInput m_gdiplusStartupInput;

// 实现

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CSearchAlgApp theApp;