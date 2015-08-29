
// SearchAlgDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include <vector>
#include "MyPoint.h"
#include "CfpResultDlg.h"
#include "gdiplus.h"//引入GDI+头文件

using namespace Gdiplus;//使用GDI+命名空间
using namespace std;
#pragma comment(lib, "gdiplus.lib")

#define ID_TIMER1 1//设置定时器ID

// CSearchAlgDlg 对话框
class CSearchAlgDlg : public CDialogEx
{
// 构造
public:
	CSearchAlgDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CSearchAlgDlg();//我自己添加的析构函数，原来并没有

// 对话框数据
	enum { IDD = IDD_SEARCHALG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAbout();
	afx_msg void OnBnClickedSetmissionarea();
	afx_msg void OnBnClickedSetsearchparam();
	afx_msg void OnBnClickedBeginsearch();
private:
	//东经起始点
	double m_LongtitudeStart;
	//北纬起始点
	double m_LatitudeStart;
	//东经结束点
	double m_longtitudeEnd;
	//北纬结束点
	double m_LatitudeEnd;
private:
	//任务区域的大小，单位km
	double m_axisX;
	double m_axisY;
private:
	//现实世界与屏幕映射的标度尺
	double m_ratioX;
	double m_ratioY;
	int m_xPadding;
	int m_yPadding;

private:
	// 发现概率
	double m_findProb;
	// 搜索宽度
	double m_searchWidth;
	double m_flightLength;//航线总长度
private:
	double GetSearchWidth(double findProb);//通过发现概率获取搜索宽度
	CComboBox m_searchMode;
	CfpResultDlg *m_fpResDlg;
private:
	vector<CPoint> m_fPSetScreen;//航点集合（针对屏幕坐标系）
	vector<MyPoint> m_fPSetReal;//航点集合（针对真实坐标系）
	void GetFPSetGrid();//栅形搜索
	void GetFPSetSquare();//方形搜索
	double GetFLenGrid(const vector<MyPoint> &fPSetReal);//获取栅形搜索航线总长度

	// X轴网格个数
	UINT m_xDivNum;
	// Y轴网格个数
	UINT m_yDivNum;
	// 漏搜率
	CString m_leakSweep;
	// 复搜率
	CString m_doubleSweep;
	int m_nFPts;//记录当前画到哪一个点
	CPoint m_currPt;//记录飞机所在点坐标值
	int m_stepPixel;//每隔一个定时器间隔步进的像素点数
	int m_periodMS;//动画绘制周期(ms)
	int m_routeAlpha;//区域透明度，0-255
	COLORREF m_areaColor;//区域颜色
	
	bool m_missionAreaFlag;//是否成功生成任务区域
	bool m_searchParamFlag;//是否成功设定搜索参数

public:
	afx_msg void OnBnClickedfpresult();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSetanime();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
