
// SearchAlgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SearchAlg.h"
#include "SearchAlgDlg.h"
#include "afxdialogex.h"
#include "math.h"
#include "DialogAnime.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define EARTHRADIUS (6371) //单位：km
#define PI (3.1415926535898)
#define MAINAREA_BG (RGB(0, 176, 240))

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSearchAlgDlg 对话框



CSearchAlgDlg::CSearchAlgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSearchAlgDlg::IDD, pParent)
	, m_LongtitudeStart(0)
	, m_LatitudeStart(0)
	, m_longtitudeEnd(0)
	, m_LatitudeEnd(0)
	, m_findProb(0)
	, m_searchWidth(0)
	, m_xDivNum(0)
	, m_yDivNum(0)
	, m_leakSweep(_T(""))
	, m_doubleSweep(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(1);
	//用户自定义初始化数据
	m_axisX = 0;
	m_axisY = 0;
	m_nFPts = 0;
	m_currPt = CPoint(0,0);//当前坐标初始值
	m_stepPixel = 5;
	m_periodMS = 5;
	m_routeAlpha = 90;
	m_missionAreaFlag = false;
	m_searchParamFlag = false;
}

//析构函数
CSearchAlgDlg::~CSearchAlgDlg()
{
	if (NULL != m_fpResDlg)//如果非模态对话框已创建则销毁他
		delete m_fpResDlg;
}

void CSearchAlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LongtitudeStart, m_LongtitudeStart);
	DDV_MinMaxDouble(pDX, m_LongtitudeStart, 0, 180);
	DDX_Text(pDX, IDC_LatitudeStart, m_LatitudeStart);
	DDV_MinMaxDouble(pDX, m_LatitudeStart, 0, 90);
	DDX_Text(pDX, IDC_LongtitudeEnd, m_longtitudeEnd);
	DDV_MinMaxDouble(pDX, m_longtitudeEnd, 0, 180);
	DDX_Text(pDX, IDC_LatitudeEnd, m_LatitudeEnd);
	DDV_MinMaxDouble(pDX, m_LatitudeEnd, 0, 90);
	DDX_Text(pDX, IDC_FindProb, m_findProb);
	DDV_MinMaxDouble(pDX, m_findProb, 0, 1);
	DDX_Text(pDX, IDC_SearchWidth, m_searchWidth);
	DDX_Control(pDX, IDC_SearchMode, m_searchMode);
	DDX_Text(pDX, IDC_XDivNum, m_xDivNum);
	DDV_MinMaxUInt(pDX, m_xDivNum, 2, 8);
	DDX_Text(pDX, IDC_YDivNum, m_yDivNum);
	DDV_MinMaxUInt(pDX, m_yDivNum, 2, 8);
	DDX_Text(pDX, IDC_LeakSweep, m_leakSweep);
	DDX_Text(pDX, IDC_DoubleSweep, m_doubleSweep);
}

BEGIN_MESSAGE_MAP(CSearchAlgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_About, &CSearchAlgDlg::OnAbout)
	ON_BN_CLICKED(IDC_SetMissionArea, &CSearchAlgDlg::OnBnClickedSetmissionarea)
	ON_BN_CLICKED(IDC_SetSearchParam, &CSearchAlgDlg::OnBnClickedSetsearchparam)
	ON_BN_CLICKED(IDC_BeginSearch, &CSearchAlgDlg::OnBnClickedBeginsearch)
	ON_BN_CLICKED(IDC_fPResult, &CSearchAlgDlg::OnBnClickedfpresult)
	ON_WM_TIMER()
	ON_COMMAND(ID_SetAnime, &CSearchAlgDlg::OnSetanime)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CSearchAlgDlg 消息处理程序

BOOL CSearchAlgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	//m_searchMode.SetCurSel(0); //设置搜索模式下拉框为第零个元素
	
	//设定网格段数默认值
	m_xDivNum = 6;
	m_yDivNum = 6;
	UpdateData(FALSE);
	GetDlgItem(IDC_fPResult)->EnableWindow(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSearchAlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSearchAlgDlg::OnPaint()
{
	if (IsIconic())//判断窗口是否处于最小化状态（点击了最小化按钮之后),
		//当你最小化了对话框之后，虽然IsIconic()的值是TRUE，但是OnPaint()函数并不会运行。
		//因为OnPaint()响应的是WM_PAINT消息，而WM_PAINT消息是针对客户区的。一个最小化了的窗口不需要重绘客户区。
		//一般没啥大用的样子
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		//------------我的绘图区------------
		CWnd *pWnd = GetDlgItem(IDC_MainArea);
		CDC *pDC = pWnd->GetDC();
		CRect mainAreaRect;
		GetDlgItem(IDC_MainArea)->GetClientRect(mainAreaRect);


		//采用双缓冲的方式重绘MainArea区
		CDC MemDC; //首先定义一个显示设备对象
		CBitmap MemBitmap;//定义一个位图对象
		//随后建立与屏幕显示兼容的内存显示设备
		MemDC.CreateCompatibleDC(NULL);
		//-----------这时还不能绘图，因为没有地方画 ^_^----------------
		//下面建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小
		MemBitmap.CreateCompatibleBitmap(pDC, mainAreaRect.Width(), mainAreaRect.Height());
		//将位图选入到内存显示设备中
		//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
		CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
		//先用背景色将位图清除干净，这里我用的是白色作为背景
		//你也可以用自己应该用的颜色
		MemDC.FillSolidRect(0, 0, mainAreaRect.Width(), mainAreaRect.Height(), MAINAREA_BG);
		//-------------------------绘图---------------------------------

		pWnd->Invalidate();//将特定区域置为无效区，等待刷新
		//主窗口刷新的时候，是不会刷新大多数控件区域的，静态文本等部分控件除外
		pWnd->UpdateWindow();
		CBrush brs;
		brs.CreateSolidBrush(MAINAREA_BG);
		pDC->FillRect(&mainAreaRect, &brs);
		pDC->SetBkMode(TRANSPARENT);//设置字体背景颜色为透明

		//-------------------------定义画笔-----------------------------
		CPen *pPenRed = new CPen();
		pPenRed->CreatePen(PS_DOT, 5, RGB(255, 0, 0));//红色画笔
		CPen* pPenBlack = new CPen();
		pPenBlack->CreatePen(PS_SOLID, 3, RGB(0, 0, 0));//黑色画笔
		CPen* pPenGray = new CPen();
		pPenGray->CreatePen(PS_DOT, 1, RGB(128, 128, 128));//灰色画笔
		CPen *pPenBG = new CPen();
		pPenBG->CreatePen(PS_DOT, 1, MAINAREA_BG);//任务区域背景颜色画笔

		//--------画栅形坐标图------------
		if (m_searchMode.GetCurSel() == 0){//栅形主界面
			//可考虑用mainAreaRect.Width()与mainAreaRect.Height()作为axisX与axisY
			int axisX = mainAreaRect.Width();
			int axisY = mainAreaRect.Height();
			int XPadding = 60;//坐标轴离左上角的间隔
			int YPadding = 50;
			m_xPadding = XPadding;
			m_yPadding = YPadding;
			int xDivisionNum = m_xDivNum;
			int yDivisionNum = m_yDivNum;
			//选中黑色画笔，并保存当前画笔为pOldPen
			CGdiObject* pOldPen = pDC->SelectObject(pPenBlack);
			pDC->MoveTo(XPadding, YPadding);//设定原点
			pDC->LineTo(XPadding, axisY - 30);//垂直轴
			pDC->MoveTo(XPadding, YPadding);//回原点
			pDC->LineTo(axisX - 30, YPadding);//水平轴
			//横坐标箭头
			pDC->MoveTo(axisX - 30 - 5, YPadding - 5);
			pDC->LineTo(axisX - 30, YPadding);
			pDC->MoveTo(axisX - 30 - 5, YPadding + 5);
			pDC->LineTo(axisX - 30, YPadding);
			////纵坐标箭头
			pDC->MoveTo(XPadding - 5, axisY - 30 - 3);
			pDC->LineTo(XPadding, axisY - 30);
			pDC->MoveTo(XPadding + 5, axisY - 30 - 3);
			pDC->LineTo(XPadding, axisY - 30);

			CString str;
			int i = 0;
			//写x轴刻度值单位
			pDC->TextOut(axisX - 50, YPadding + 10, _T("X(km)"));
			//写y轴刻度值单位
			pDC->TextOut(XPadding - 25, axisY - 25, _T("Y(km)"));

			//网格线
			int nMeshLengthX = (axisX - XPadding - 10) / (xDivisionNum + 1);
			int nMeshLengthY = (axisY - YPadding - 10) / (yDivisionNum + 1);
			pDC->SelectObject(pPenGray);
			for (i = 1; i<xDivisionNum; i++){
				pDC->MoveTo(XPadding + i*nMeshLengthX, YPadding);
				pDC->LineTo(XPadding + i*nMeshLengthX, axisY - 40);
			}
			//最后一根实线用红色表示
			pDC->SelectObject(pPenRed);
			pDC->MoveTo(XPadding + xDivisionNum*nMeshLengthX, YPadding);
			pDC->LineTo(XPadding + xDivisionNum*nMeshLengthX, axisY - 40);
			pDC->SelectObject(pPenGray);

			nMeshLengthY = (axisY - YPadding - 10) / (yDivisionNum + 1);
			for (i = 1; i<yDivisionNum; i++){
				pDC->MoveTo(XPadding, YPadding + i*nMeshLengthY);
				pDC->LineTo(axisX - 40, YPadding + i*nMeshLengthY);
			}
			//最后一根实线
			pDC->SelectObject(pPenRed);
			pDC->MoveTo(XPadding, YPadding + yDivisionNum*nMeshLengthY);
			pDC->LineTo(axisX - 40, YPadding + yDivisionNum*nMeshLengthY);
			//绘制区域边界的交点
			pDC->SelectObject(pPenBlack);
			str.Format(L"P(%.1f,%.1f)", m_axisX, m_axisY);
			int wallX = XPadding + xDivisionNum*nMeshLengthX;
			int wallY = YPadding + yDivisionNum*nMeshLengthY;
			m_ratioX = (wallX - XPadding) / m_axisX;
			m_ratioY = (wallY - YPadding) / m_axisY;

			pDC->TextOut(XPadding + xDivisionNum*nMeshLengthX - 45, YPadding + yDivisionNum*nMeshLengthY + 10, str);
			pDC->Ellipse(wallX - 5, wallY - 5, wallX + 5, wallY + 5);


			//画出横坐标数字
			double xRealDiv = m_axisX / xDivisionNum;
			for (i = 0; i < xDivisionNum + 1; i++){
				str.Format(L"%.1f", i*xRealDiv);
				pDC->TextOut(XPadding + i*nMeshLengthX - 10, YPadding - 25, str);
			}
			//画出纵坐标数字
			double yRealDiv = m_axisY / yDivisionNum;
			for (i = 0; i < yDivisionNum + 1; i++){
				str.Format(L"%.1f", i*yRealDiv);
				pDC->TextOut(XPadding - 45, YPadding + i*nMeshLengthY - 8, str);
			}
			//恢复以前的画笔
			pDC->SelectObject(pOldPen);
			ReleaseDC(pDC);
			//----------------------------
		}
		else if (1 == m_searchMode.GetCurSel()){//方形搜索，需要限定输入的经纬度差相等才能保证任务区域为方形
			if (m_longtitudeEnd - m_LongtitudeStart != m_LatitudeEnd - m_LatitudeStart){
				MessageBox(L"警告：任务区域为方形，经纬度差必须相等！");
				return;
			}
			int axisX = mainAreaRect.Width();
			int axisY = mainAreaRect.Height();
			int XPadding = 30;//坐标轴离左右的间隔
			int YPadding = 25;//坐标轴离上下的间隔
			m_xPadding = XPadding;
			m_yPadding = YPadding;
			int xDivisionNum = m_xDivNum;
			int yDivisionNum = m_yDivNum;
			//选中黑色画笔，并保存当前画笔为pOldPen
			CGdiObject* pOldPen = pDC->SelectObject(pPenBlack);
			int centerX = (axisX - 2 * XPadding) / 2 + XPadding;//中心坐标x
			int centerY = (axisY - 2 * YPadding) / 2 + YPadding;//中心坐标y
			pDC->MoveTo(XPadding, centerY);
			pDC->LineTo(axisX - XPadding, centerY);//水平轴
			pDC->MoveTo(centerX, axisY-YPadding);
			pDC->LineTo(centerX, YPadding);//垂直轴
			//横坐标箭头
			pDC->MoveTo(axisX - XPadding - 5, centerY - 5);
			pDC->LineTo(axisX - XPadding, centerY);
			pDC->MoveTo(axisX - XPadding - 5, centerY + 5);
			pDC->LineTo(axisX - XPadding, centerY);
			////纵坐标箭头
			pDC->MoveTo(centerX - 5, YPadding + 5);
			pDC->LineTo(centerX, YPadding);
			pDC->MoveTo(centerX + 5, YPadding + 5);
			pDC->LineTo(centerX, YPadding);

			CString str;
			int i = 0;
			//写x轴刻度值单位
			pDC->TextOut(axisX - XPadding-20, centerY-28, _T("X(km)"));
			//写y轴刻度值单位
			pDC->TextOut(centerX-10, YPadding - 25, _T("Y(km)"));
			//x轴网格线
			int nMeshLengthX = (axisX - 2*XPadding - 10) / (xDivisionNum + 1);
			int nMeshLengthY = (axisY - 2*YPadding - 10) / (yDivisionNum + 1);
			pDC->SelectObject(pPenGray);
			for (i = 1; i<xDivisionNum/2; i++){//从中心向右延伸
				pDC->MoveTo(centerX + i*nMeshLengthX, YPadding);
				pDC->LineTo(centerX + i*nMeshLengthX, axisY - YPadding);
			}
			//最后一根实线用红色表示
			pDC->SelectObject(pPenRed);
			pDC->MoveTo(centerX + xDivisionNum*nMeshLengthX / 2, YPadding);
			pDC->LineTo(centerX + xDivisionNum*nMeshLengthX / 2, axisY - YPadding);
			pDC->SelectObject(pPenGray);
			for (i = 1; i<xDivisionNum / 2; i++){//从中心向左延伸
				pDC->MoveTo(centerX - i*nMeshLengthX, YPadding);
				pDC->LineTo(centerX - i*nMeshLengthX, axisY - YPadding);
			}
			pDC->SelectObject(pPenRed);
			pDC->MoveTo(centerX - xDivisionNum*nMeshLengthX / 2, YPadding);
			pDC->LineTo(centerX - xDivisionNum*nMeshLengthX / 2, axisY - YPadding);
			//y轴网格线
			pDC->SelectObject(pPenGray);
			for (i = 1; i<yDivisionNum / 2; i++){//从中心向上延伸
				pDC->MoveTo(XPadding, centerY - i*nMeshLengthY);
				pDC->LineTo(axisX - XPadding, centerY - i*nMeshLengthY);
			}
			pDC->SelectObject(pPenRed);
			pDC->MoveTo(XPadding, centerY - yDivisionNum*nMeshLengthY / 2);
			pDC->LineTo(axisX - XPadding, centerY - yDivisionNum*nMeshLengthY / 2);
			pDC->SelectObject(pPenGray);
			for (i = 1; i<yDivisionNum / 2; i++){//从中心向下延伸
				pDC->MoveTo(XPadding, centerY + i*nMeshLengthY);
				pDC->LineTo(axisX - XPadding, centerY + i*nMeshLengthY);
			}
			pDC->SelectObject(pPenRed);
			pDC->MoveTo(XPadding, centerY + yDivisionNum*nMeshLengthY / 2);
			pDC->LineTo(axisX - XPadding, centerY + yDivisionNum*nMeshLengthY / 2);

			//绘制区域边界的交点
			int wallX = xDivisionNum*nMeshLengthX;
			int wallY = yDivisionNum*nMeshLengthY;
			m_ratioX = wallX / m_axisX;
			m_ratioY = wallY / m_axisY;
			pDC->SelectObject(pPenBlack);
			//描点P1
			str.Format(L"P1(%.1f,%.1f)", -m_axisX / 2, m_axisY / 2);
			pDC->TextOut((axisX-wallX)/2-45, (axisY-wallY)/2-30 , str);
			pDC->Ellipse((axisX - wallX) / 2 - 5, (axisY - wallY) / 2 - 5, (axisX - wallX) / 2 + 5, (axisY - wallY) / 2 + 5);
			//描点P2
			str.Format(L"P2(%.1f,%.1f)", m_axisX / 2, m_axisY / 2);
			pDC->TextOut((axisX - wallX) / 2 + wallX - 65, (axisY - wallY) / 2 - 30, str);
			pDC->Ellipse((axisX - wallX) / 2 + wallX - 5, (axisY - wallY) / 2 - 5, (axisX - wallX) / 2 + wallX + 5, (axisY - wallY) / 2 + 5);
			//描点P3
			str.Format(L"P3(%.1f,%.1f)", m_axisX / 2, -m_axisY / 2);
			pDC->TextOut((axisX - wallX) / 2 + wallX - 65, (axisY - wallY) / 2 + wallY + 10, str);
			pDC->Ellipse((axisX - wallX) / 2 + wallX - 5, (axisY - wallY) / 2 + wallY - 5, (axisX - wallX) / 2 + wallX + 5, (axisY - wallY) / 2 + wallY + 5);
			//描点P4
			str.Format(L"P4(%.1f,%.1f)", -m_axisX / 2, -m_axisY / 2);
			pDC->TextOut((axisX - wallX) / 2 - 45, (axisY - wallY) / 2 + wallY + 10, str);
			pDC->Ellipse((axisX - wallX) / 2 - 5, (axisY - wallY) / 2 + wallY - 5, (axisX - wallX) / 2 + 5, (axisY - wallY) / 2 + wallY + 5);

			//画出横坐标数字
			double xRealDiv = m_axisX / xDivisionNum;
			for (i = 1; i <= xDivisionNum/2; i++){//从中心向右标数字
				str.Format(L"%.1f", i*xRealDiv);
				pDC->TextOut(centerX + i*nMeshLengthX - 15, centerY + 10, str);
			}
			for (i = 1; i <= xDivisionNum/2; i++){//从中心向左标数字
				str.Format(L"%.1f", -i*xRealDiv);
				pDC->TextOut(centerX - i*(nMeshLengthX+8) - 10, centerY + 10, str);
			}
			//画出纵坐标数字
			double yRealDiv = m_axisY / yDivisionNum;
			for (i = 1; i <= yDivisionNum/2; i++){//从中心向上标数字
				str.Format(L"%.1f", i*yRealDiv);
				pDC->TextOut(centerX+5, centerY - i*nMeshLengthY - 10, str);
			}
			for (i = 1; i <= yDivisionNum / 2; i++){//从中心向下标数字
				str.Format(L"%.1f", -i*yRealDiv);
				pDC->TextOut(centerX +5, centerY + i*nMeshLengthY - 10, str);
			}
			//画出原点
			str.Format(L"0.0");
			pDC->TextOut(centerX +5, centerY+5, str);

			//恢复以前的画笔
			pDC->SelectObject(pOldPen);
			ReleaseDC(pDC);
		}

		
		delete pPenBlack;
		delete pPenRed;
		delete pPenGray;
		delete pPenBG;
		//--------------------------------------------------------------

		//将内存中的图拷贝到屏幕上进行显示
		pDC->BitBlt(0, 0, mainAreaRect.Width(), mainAreaRect.Height(), &MemDC, 0, 0, SRCCOPY);
		//绘图完成后的清理
		MemBitmap.DeleteObject();
		MemDC.DeleteDC();
		ReleaseDC(pDC);
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSearchAlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//通过发现概率获取搜索宽度，注意P在(0,1)之间
double CSearchAlgDlg::GetSearchWidth(double findProb)
{
	double deltaPow = 375 / (2 * (log(double(5)) - log(double(3))));
	double omega = sqrt(25 - 2 * deltaPow * log(findProb));
	return 2*omega;
}

//获取航行路点至m_flightPointSet中，栅形模式
void CSearchAlgDlg::GetFPSetGrid()
{
	m_fPSetReal.clear();//真实坐标，用于描点显示（里面含double数据）
	m_fPSetScreen.clear();//屏幕坐标（里面只有int型数据）
	
	double W = m_searchWidth;

	m_fPSetReal.push_back(MyPoint(0, W/2));//A0，进入点
	m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Grid(m_ratioX, m_ratioY, m_xPadding, m_yPadding));//A0，进入点

	int i = 1;
	while (true){
		//先计算出A(8i-7)到A(8i)，再依据相应的情况pop一些出去
		m_fPSetReal.push_back(MyPoint(m_axisX - 1 - W / 2, W / 2 + (2 * i - 2)*W));//A(8i-7)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Grid(m_ratioX, m_ratioY, m_xPadding, m_yPadding));

		m_fPSetReal.push_back(MyPoint(m_axisX - W / 2, W / 2 + 1 + (2 * i - 2)*W));//A(8i-6)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Grid(m_ratioX, m_ratioY, m_xPadding, m_yPadding));

		m_fPSetReal.push_back(MyPoint(m_axisX - W / 2, W / 2 - 1 + (2 * i - 1)*W));//A(8i-5)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Grid(m_ratioX, m_ratioY, m_xPadding, m_yPadding));

		m_fPSetReal.push_back(MyPoint(m_axisX - 1 - W / 2, W / 2 + (2 * i - 1)*W));//A(8i-4)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Grid(m_ratioX, m_ratioY, m_xPadding, m_yPadding));

		m_fPSetReal.push_back(MyPoint(W / 2+1, W/2 + (2 * i - 1)*W));//A(8i-3)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Grid(m_ratioX, m_ratioY, m_xPadding, m_yPadding));

		m_fPSetReal.push_back(MyPoint(W / 2 , W / 2 + 1 + (2 * i-1) *W));//A(8i-2)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Grid(m_ratioX, m_ratioY, m_xPadding, m_yPadding));

		m_fPSetReal.push_back(MyPoint(W / 2 ,  W / 2 -1 + 2 * i *W));//A(8i-1)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Grid(m_ratioX, m_ratioY, m_xPadding, m_yPadding));

		m_fPSetReal.push_back(MyPoint(W / 2 + 1, W / 2 + 2 * i *W));//A(8i)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Grid(m_ratioX, m_ratioY, m_xPadding, m_yPadding));

		if ((2 * i - 1)*W<m_axisY && 2 * i*W>m_axisY){//转弯点计算到A(8i-4)为止，退出点为(0,W/2+(2i-1)W)
			for (int j = 0; j < 4; j++){
				m_fPSetReal.pop_back();
				m_fPSetScreen.pop_back();
			}
			//压入退出点，退出点作为数组的最后一个元素
			m_fPSetReal.push_back(MyPoint(0, W / 2 + (2 * i - 1)*W));
			m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Grid(m_ratioX, m_ratioY, m_xPadding, m_yPadding));
			break;

		}
		else if (2*i*W<m_axisY && (2*i+1)*W>m_axisY){//转弯点计算到A(8i)为止，退出点为(x1,W/2+2iW)
			//压入退出点，退出点作为数组的最后一个元素
			m_fPSetReal.push_back(MyPoint(m_axisX, W / 2 + 2 * i *W));
			m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Grid(m_ratioX, m_ratioY, m_xPadding, m_yPadding));
			break;
		}

		i++;
	}


}
//获取航行路点至m_flightPointSet中，方形模式
void CSearchAlgDlg::GetFPSetSquare()
{
	m_fPSetReal.clear();//真实坐标，用于描点显示（里面含double数据）
	m_fPSetScreen.clear();//屏幕坐标（里面只有int型数据）

	double W = m_searchWidth;
	CRect mainAreaRect;
	GetDlgItem(IDC_MainArea)->GetClientRect(mainAreaRect);
	int centerX = (mainAreaRect.Width() - 2 * m_xPadding) / 2 + m_xPadding;//中心坐标x
	int centerY = (mainAreaRect.Height() - 2 * m_yPadding) / 2 + m_yPadding;//中心坐标y

	m_fPSetReal.push_back(MyPoint(0, 0));//A0，进入点
	m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Square(m_ratioX, m_ratioY, m_xPadding, m_yPadding, centerX, centerY));//A0，进入点

	//计算截止的k值
	/*int k = -1;
	if (m_axisX / (2 * W) + 0.5 == ceil(m_axisX / (2 * W))){
		k = int(floor(m_axisX / (2 * W)));
	}
	else if (m_axisX / (2 * W) + 0.5>ceil(m_axisX / (2 * W))){
		k = int(ceil(m_axisX / (2 * W)));
	}
	else{
		k = int(floor(m_axisX / (2 * W)));
	}*/

	int k = int(floor(1 + m_axisX / (2 * W)));

	for (int i = 1; i <= k;i++)
	{
		//先计算出A(8i-7)到A(8i)，再依据相应的情况pop一些出去
		m_fPSetReal.push_back(MyPoint(i*W - W/2 - 1 , (i-1)*W));//A(8i-7)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Square(m_ratioX, m_ratioY, m_xPadding, m_yPadding, centerX, centerY));

		m_fPSetReal.push_back(MyPoint(i*W - W / 2, (i - 1)*W - 1));//A(8i-6)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Square(m_ratioX, m_ratioY, m_xPadding, m_yPadding, centerX, centerY));

		m_fPSetReal.push_back(MyPoint(i*W - W / 2, 1-i*W ));//A(8i-5)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Square(m_ratioX, m_ratioY, m_xPadding, m_yPadding, centerX, centerY));

		m_fPSetReal.push_back(MyPoint(i*W - 1 - W / 2, -i*W));//A(8i-4)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Square(m_ratioX, m_ratioY, m_xPadding, m_yPadding, centerX, centerY));

		m_fPSetReal.push_back(MyPoint(W / 2 - i*W + 1, -i*W));//A(8i-3)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Square(m_ratioX, m_ratioY, m_xPadding, m_yPadding, centerX, centerY));

		m_fPSetReal.push_back(MyPoint(W / 2 - i*W, 1-i*W));//A(8i-2)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Square(m_ratioX, m_ratioY, m_xPadding, m_yPadding, centerX, centerY));

		m_fPSetReal.push_back(MyPoint(W / 2 - i*W, i*W - 1));//A(8i-1)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Square(m_ratioX, m_ratioY, m_xPadding, m_yPadding, centerX, centerY));

		m_fPSetReal.push_back(MyPoint(W / 2 - i*W + 1, i*W));//A(8i)
		m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Square(m_ratioX, m_ratioY, m_xPadding, m_yPadding, centerX, centerY));
	
	}
	//转弯点计算到A(8i)为止，退出点为(a,iW)
	//压入退出点，退出点作为数组的最后一个元素
	m_fPSetReal.push_back(MyPoint(m_axisX / 2, k *W));
	m_fPSetScreen.push_back(m_fPSetReal.back().CvtToCP_Square(m_ratioX, m_ratioY, m_xPadding, m_yPadding, centerX, centerY));
}

//通过航路点获取航线总长度
double CSearchAlgDlg::GetFLenGrid(const vector<MyPoint> &fPSetReal)
{
	double flightLen = 0.0;
	//先计算1/4圆弧的弧长
	double arcLen = (1 * 2 * PI) / 4;
	//几何距离和弧长
	for (size_t i = 0; i < fPSetReal.size()-1; i++)
	{
		if (i%2 != 0){//A(2i+1)->A(2i+2)这种，就直接加上弧长并跳到下一步
			flightLen += arcLen;
		}
		else{//否则累加几何距离
			flightLen += sqrt(pow(fPSetReal[i + 1].x - fPSetReal[i].x, 2) + pow(fPSetReal[i + 1].y - fPSetReal[i].y, 2));
		}

	}
	return flightLen;
}

void CSearchAlgDlg::OnAbout()
{
	// TODO:  在此添加命令处理程序代码
	CDialog *aboutDlg = new CDialog();
	aboutDlg->Create(MAKEINTRESOURCE(IDD_ABOUTBOX));
	aboutDlg->ShowWindow(true);
}


void CSearchAlgDlg::OnBnClickedSetmissionarea()
{
	// TODO:  在此添加控件通知处理程序代码
	GetDlgItem(IDC_fPResult)->EnableWindow(FALSE);

	UpdateData(TRUE);//将EditControl的值取出放入关联的Value变量中
	if (m_LongtitudeStart < 0 || m_LatitudeStart < 0){//经纬度下溢异常
		MessageBox(L"警告：起始经纬度不能小于0！");
		m_missionAreaFlag = false;
		return;
	}
	else if (m_longtitudeEnd > 180 || m_LatitudeEnd > 90){//经纬度上溢异常
		MessageBox(L"警告：结束经纬度不能超出范围！");
		m_missionAreaFlag = false;
		return;
	}
	else if (m_longtitudeEnd - m_LongtitudeStart <= 0 || m_LatitudeEnd - m_LatitudeStart<=0){//起始经纬度小于结束经纬度
		MessageBox(L"警告：结束经纬度小于或等于起始经纬度！");
		m_missionAreaFlag = false;
		return;
	}
	else{//正常情况，每次点击都在MissionArea产生新的坐标刻度
		if (0 == m_searchMode.GetCurSel()){//栅形搜索
			
		}
		else if (1 == m_searchMode.GetCurSel()){//方形搜索，需要限定输入的经纬度差相等才能保证任务区域为方形
			if (m_longtitudeEnd - m_LongtitudeStart != m_LatitudeEnd - m_LatitudeStart){
				MessageBox(L"警告：任务区域为方形，经纬度差必须相等！");
				return;
			}
			UpdateData(TRUE);
			if (m_xDivNum % 2 != 0){//如果不是偶数
				MessageBox(L"警告：方形模式下的网格数必须为偶数！");
				return;
			}
		}
		else{
			MessageBox(L"警告：未选择搜索模式！");
		}
		//两种搜索模式都需要计算这些值
		m_axisX = 2 * PI*EARTHRADIUS*(m_longtitudeEnd - m_LongtitudeStart) / 360;
		m_axisY = 2 * PI*EARTHRADIUS*(m_LatitudeEnd - m_LatitudeStart) / 360;
		//保留小数点后两位
		m_axisX = floor(m_axisX*100.0f + 0.5) / 100.0f;//保留小数点后两位
		m_axisY = floor(m_axisY*100.0f + 0.5) / 100.0f;
		//主区域的面积至少大于240km才能得到较低的复搜率和漏搜率
		if ((m_longtitudeEnd - m_LongtitudeStart < (240 * 360 / (2 * PI*EARTHRADIUS))) || 
								(m_LatitudeEnd - m_LatitudeStart < (240 * 360 / (2 * PI*EARTHRADIUS)))){
			MessageBox(L"警告：任务区域边界不能小于240km！");
			m_missionAreaFlag = false;
			return;
		}
		
		m_missionAreaFlag = true;
		//产生主界面
		Invalidate();//void Invalidate( BOOL bErase = TRUE );参数: bErase 决定了是否要在WM_PAINT消息前发送WM_ERASEBKGND
		//也就是，在默认情况下（TRUE），先发送背景刷新消息，再刷新前景。而FALSE只刷新前景，也就是只触发OnPaint函数。
		UpdateWindow();
	}
}

void CSearchAlgDlg::OnBnClickedSetsearchparam()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_findProb <= 0 || m_findProb >= 1){//不允许发现概率等于0或1，只能接近
		MessageBox(L"警告：发现概率输入错误！");
		m_searchParamFlag = false;
		return;
	}
	else{
		m_searchWidth = GetSearchWidth(m_findProb);
		//保留小数点后一位有效数字，并写入搜索宽度
		m_searchWidth = floor(m_searchWidth*100.0f + 0.5) / 100.0f;//保留小数点后两位
		UpdateData(FALSE);
		m_searchParamFlag = true;
	}
}


void CSearchAlgDlg::OnBnClickedBeginsearch()
{
	// TODO:  在此添加控件通知处理程序代码

	//判断参数是否设置正确
	if (false == m_missionAreaFlag ){
		MessageBox(L"警告：未设定好任务区域！");
		return;
	}
	if (false == m_searchParamFlag){
		MessageBox(L"警告：未设定好搜索参数！");
		return;
	}

	Invalidate();
	UpdateWindow();
	m_leakSweep = "";
	m_doubleSweep = "";
	UpdateData(FALSE);
	
	//启动定时器，开始演示搜索过程
	SetTimer(ID_TIMER1, m_periodMS, NULL);
	GetDlgItem(IDC_BeginSearch)->EnableWindow(FALSE);
	GetDlgItem(IDC_fPResult)->EnableWindow(FALSE);
	GetDlgItem(IDC_SetMissionArea)->EnableWindow(FALSE);
	GetDlgItem(IDC_SetSearchParam)->EnableWindow(FALSE);


	//查看选中的搜索模式，并开始搜索
	if (0 == m_searchMode.GetCurSel()){//栅形搜索
		//MessageBox(L"栅形");
		GetFPSetGrid();//获取栅形转弯点对
		//更新漏搜率和复搜率，注意原公式的c是点集剔除最后一个退出点后以及起始点的航点个数
		double W = m_searchWidth;
		double leakS = (1 - PI / 4)*pow((1 + W / 2), 2)*(m_fPSetScreen.size() - 2) / 2;
		double leakSweep = leakS / (m_axisX*m_axisY) * 100;
		double doubleSweep = (W*GetFLenGrid(m_fPSetReal) - m_axisX*(m_fPSetReal.back().y + W / 2) + leakS) / (m_axisX*m_axisY) * 100;

		m_leakSweep.Format(L"%.2f %%", leakSweep);
		m_doubleSweep.Format(L"%.2f %%", doubleSweep);
		
	}
	else if (1 == m_searchMode.GetCurSel()){//方形搜索
		//MessageBox(L"方形");
		GetFPSetSquare();//获取方形转弯点对

		double W = m_searchWidth;
		//计算航行结束时循环节的个数k
		/*int k = -1;
		if (m_axisX / (2 * W) + 0.5 == ceil(m_axisX / (2 * W))){
			k = int(floor(m_axisX / (2 * W)));
		}
		else if (m_axisX / (2 * W) + 0.5>ceil(m_axisX / (2 * W))){
			k = int(ceil(m_axisX / (2 * W)));
		}
		else{
			k = int(floor(m_axisX / (2 * W)));
		}*/
		int k = int(floor(0.5 + m_axisX / (2 * W)));

		//更新漏搜率和复搜率，注意原公式的c是点集剔除最后一个退出点后以及起始点的航点个数
		double leakS = (1 - PI / 4)*pow((1 + W / 2), 2)*(m_fPSetScreen.size() - 2) / 2;
		double leakSweep = leakS/ (m_axisX*m_axisY) * 100;

		double doubleSweep = (W*GetFLenGrid(m_fPSetReal) - (2*k*W+W)*(2*k*W) + leakS) / (m_axisX*m_axisY) * 100;

		m_leakSweep.Format(L"%.2f %%", leakSweep);
		m_doubleSweep.Format(L"%.2f %%", doubleSweep);
	}
	else{
		MessageBox(L"警告：未选择搜索模式！");
	}
	//if (m_fPSetScreen.size() == 0)//如果并没有设定模式，则直接退出
	//	return;

}


void CSearchAlgDlg::OnBnClickedfpresult()
{
	// TODO:  在此添加控件通知处理程序代码

	if (m_fPSetScreen.size() == 0)//如果并没有设定模式，则直接退出
		return;
	//显示航点对话框
	m_fpResDlg = new CfpResultDlg;
	m_fpResDlg->Create(IDD_FPResult, this);
	m_fpResDlg->ShowWindow(SW_SHOW);
	//在该对话框的编辑栏中加入相关的航点信息
	CString str;
	for (size_t i = 0; i < m_fPSetReal.size(); i++){
		if (i == 0){//航点的起始点
			str.Format(L"起始点 : (%.1f,%.1f) \r\n", m_fPSetReal[i].x, m_fPSetReal[i].y);
			m_fpResDlg->m_fPResult.SetSel(m_fpResDlg->m_fPResult.GetWindowTextLengthW(), m_fpResDlg->m_fPResult.GetWindowTextLengthW());//选中最后一个位置
			m_fpResDlg->m_fPResult.ReplaceSel(str);//用新文本替换最后一个位置
			continue;
		}
		if (i == m_fPSetReal.size() - 1){//航点到了最后结束航点
			str.Format(L"结束点 : (%.1f,%.1f)", m_fPSetReal[i].x, m_fPSetReal[i].y);
			m_fpResDlg->m_fPResult.SetSel(m_fpResDlg->m_fPResult.GetWindowTextLengthW(), m_fpResDlg->m_fPResult.GetWindowTextLengthW());//选中最后一个位置
			m_fpResDlg->m_fPResult.ReplaceSel(str);//用新文本替换最后一个位置
			break;
		}
		str.Format(L"P%d : (%.1f,%.1f) \r\n", i, m_fPSetReal[i].x, m_fPSetReal[i].y);
		m_fpResDlg->m_fPResult.SetSel(m_fpResDlg->m_fPResult.GetWindowTextLengthW(), m_fpResDlg->m_fPResult.GetWindowTextLengthW());//选中最后一个位置
		m_fpResDlg->m_fPResult.ReplaceSel(str);//用新文本替换最后一个位置
	}

	//绘制所有屏幕点
	CWnd *pWnd = GetDlgItem(IDC_MainArea);
	CDC *pDC = pWnd->GetDC();
	CPen* pPenBlue = new CPen();
	pPenBlue->CreatePen(PS_SOLID, 3, RGB(0, 0, 255));//蓝色画笔
	CGdiObject* pOldPen = pDC->SelectObject(pPenBlue);
	for (size_t i = 0; i < m_fPSetScreen.size(); i++){
		pDC->Ellipse(m_fPSetScreen[i].x - 1, m_fPSetScreen[i].y - 1, m_fPSetScreen[i].x + 1, m_fPSetScreen[i].y + 1);
	}
	//连接所有中心线
	CPen *pPenGreen = new CPen();
	pPenGreen->CreatePen(PS_SOLID, 2, RGB(80, 126, 50));
	pDC->SelectObject(pPenGreen);
	for (size_t i = 0; i < m_fPSetScreen.size()-1; i++){
		pDC->MoveTo(m_fPSetScreen[i].x, m_fPSetScreen[i].y);
		pDC->LineTo(m_fPSetScreen[i+1].x, m_fPSetScreen[i+1].y);
	}

	pDC->SelectObject(pOldPen);
	delete pPenBlue;
	ReleaseDC(pDC);
}


void CSearchAlgDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == ID_TIMER1){
		
		//描线
		CWnd *pWnd = GetDlgItem(IDC_MainArea);
		CDC *pDC = pWnd->GetDC();	

		if (m_currPt == CPoint(0, 0))//当前点为原点
			m_currPt = m_fPSetScreen[0];	

		if (m_nFPts == m_fPSetScreen.size() - 1 ){//满足截止条件就截止，并设定显示搜索结果按钮可用
			GetDlgItem(IDC_fPResult)->EnableWindow(TRUE);
			GetDlgItem(IDC_BeginSearch)->EnableWindow(TRUE);
			GetDlgItem(IDC_SetMissionArea)->EnableWindow(TRUE);
			GetDlgItem(IDC_SetSearchParam)->EnableWindow(TRUE);
			KillTimer(ID_TIMER1);
			m_nFPts = 0;
			m_currPt = CPoint(0, 0);//重置飞行相关参数
			UpdateData(FALSE);//将漏搜率和复搜率结果显示
			return;
		}

		//产生GDI+的Graphics，注意获取hdc的方法
		Gdiplus::Graphics myGraphics(pDC->m_hDC);
		//使用GDI+产生含有一定透明度的笔刷和一定透明度的画笔
		//SolidBrush myBrush(Gdiplus::Color(m_routeAlpha, 237, 125, 49));//半透明的浅色搜索区域画刷
		//Gdiplus::Pen myPen(Gdiplus::Color(m_routeAlpha, 237, 125, 49), 3);//半透明浅色轮廓
		SolidBrush myBrush(Gdiplus::Color(m_routeAlpha, 255, 255, 255));//半透明的浅色搜索区域画刷
		Gdiplus::Pen myPen(Gdiplus::Color(m_routeAlpha, 255, 255, 255), 3);//半透明浅色轮廓

		pDC->MoveTo(m_currPt.x, m_currPt.y);

		//计算搜索半径投影到屏幕上的情况
		int screenRX = int(ceil(m_searchWidth/2  * m_ratioX));
		int screenRY = int(ceil(m_searchWidth/2  * m_ratioY));

		//---------------处理下一个飞行路点的位置-----------------
		if (m_fPSetScreen[m_nFPts].x < m_fPSetScreen[m_nFPts+1].x){//向右步进
			if (m_currPt.x + m_stepPixel <= m_fPSetScreen[m_nFPts + 1].x)//没有超出范围则向右步进
				m_currPt.x = m_currPt.x + m_stepPixel;
			else{//超出当前线范围
				m_nFPts++;//跳转至下一根线，并赋值为该线端点
				m_currPt.x = m_fPSetScreen[m_nFPts].x;
				m_currPt.y = m_fPSetScreen[m_nFPts].y;
				
				myGraphics.DrawEllipse(&myPen, m_currPt.x - screenRX, m_currPt.y - screenRY, 2 * screenRX, 2 * screenRY);
				myGraphics.FillEllipse(&myBrush, m_currPt.x - screenRX, m_currPt.y - screenRY, 2 * screenRX, 2 * screenRY);
				
				return;
			}
		}
		else if (m_fPSetScreen[m_nFPts].x > m_fPSetScreen[m_nFPts+1].x){//向左步进 
			if (m_currPt.x - m_stepPixel >= m_fPSetScreen[m_nFPts + 1].x)//没有超出范围则向左步进
				m_currPt.x = m_currPt.x - m_stepPixel;
			else{//超出当前线范围
				m_nFPts++;//跳转至下一根线，并赋值为该线端点
				m_currPt.x = m_fPSetScreen[m_nFPts].x;
				m_currPt.y = m_fPSetScreen[m_nFPts].y;

				myGraphics.DrawEllipse(&myPen, m_currPt.x - screenRX, m_currPt.y - screenRY, 2 * screenRX, 2 * screenRY);
				myGraphics.FillEllipse(&myBrush, m_currPt.x - screenRX, m_currPt.y - screenRY, 2 * screenRX, 2 * screenRY);
				return;
			}
		}

		if (m_fPSetScreen[m_nFPts].y < m_fPSetScreen[m_nFPts+1].y){//向下步进
			if (m_currPt.y + m_stepPixel <= m_fPSetScreen[m_nFPts + 1].y)//没有超出范围则向下步进
				m_currPt.y = m_currPt.y + m_stepPixel;
			else{//超出当前线范围
				m_nFPts++;//跳转至下一根线，并赋值为该线端点
				m_currPt.x = m_fPSetScreen[m_nFPts].x;
				m_currPt.y = m_fPSetScreen[m_nFPts].y;

				myGraphics.DrawEllipse(&myPen, m_currPt.x - screenRX, m_currPt.y - screenRY, 2 * screenRX, 2 * screenRY);
				myGraphics.FillEllipse(&myBrush, m_currPt.x - screenRX, m_currPt.y - screenRY, 2 * screenRX, 2 * screenRY);

				return;
			}
		}
		else if (m_fPSetScreen[m_nFPts].y > m_fPSetScreen[m_nFPts+1].y){//向上步进 
			if (m_currPt.y - m_stepPixel >= m_fPSetScreen[m_nFPts + 1].y)//没有超出范围则向上步进
				m_currPt.y = m_currPt.y - m_stepPixel;
			else{//超出当前线范围
				m_nFPts++;//跳转至下一根线，并赋值为该线端点
				m_currPt.x = m_fPSetScreen[m_nFPts].x;
				m_currPt.y = m_fPSetScreen[m_nFPts].y;

				myGraphics.DrawEllipse(&myPen, m_currPt.x - screenRX, m_currPt.y - screenRY, 2 * screenRX, 2 * screenRY);
				myGraphics.FillEllipse(&myBrush, m_currPt.x - screenRX, m_currPt.y - screenRY, 2 * screenRX, 2 * screenRY);

				return;
			}
		}

		if (m_fPSetScreen[m_nFPts].x == m_fPSetScreen[m_nFPts + 1].x && m_fPSetScreen[m_nFPts].y == m_fPSetScreen[m_nFPts + 1].y){
			m_nFPts++;//跳至下一跳
		}
		
		myGraphics.DrawEllipse(&myPen, m_currPt.x - screenRX, m_currPt.y - screenRY, 2 * screenRX, 2 * screenRY);
		myGraphics.FillEllipse(&myBrush, m_currPt.x - screenRX, m_currPt.y - screenRY, 2 * screenRX, 2 * screenRY);
		
	
		ReleaseDC(pDC);
	}
	else{
		//处理另外其他未预设的Timer
	}
	CDialogEx::OnTimer(nIDEvent);
}

//设定了动画效果参数
void CSearchAlgDlg::OnSetanime()
{
	// TODO:  在此添加命令处理程序代码
	CDialogAnime DlgAnime;
	DlgAnime.m_periodMS = m_periodMS;
	DlgAnime.m_pixelStep = m_stepPixel;
	DlgAnime.m_routeAlpha = m_routeAlpha;

	if (IDOK == DlgAnime.DoModal()){//按下了ok键
		m_stepPixel = DlgAnime.m_pixelStep;
		m_periodMS = DlgAnime.m_periodMS;
		m_routeAlpha = DlgAnime.m_routeAlpha;
	}

}


void CSearchAlgDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CRect rect_ctr;
	//获取Picture控件相对屏幕左上角的坐标，/存储到rect_ctr中
	GetDlgItem(IDC_MainArea)->GetWindowRect(&rect_ctr);
	CDC *pDC = GetDlgItem(IDC_MainArea)->GetDC();

	ScreenToClient(rect_ctr); //获取Picture控件相对对话框客户区左上角的坐标
	point.x -= rect_ctr.left; //point获取的是鼠标相对对话框客户区左上角的坐标，减去rect_ctr.left和
	point.y -= rect_ctr.top; //rect_ctr.top后，即为鼠标相对Picture控件左上角的坐标
	
	CString str;
	//如果鼠标位于Picture控件之外，在对话框上不显示坐标
	if (point.x < 0 || point.y<0 || point.x + rect_ctr.left>rect_ctr.right || point.y + rect_ctr.top>rect_ctr.bottom || m_ratioX==0 || m_ratioY == 0){
		;
	}
	else{
		double xReal, yReal;
		if (m_searchMode.GetCurSel() == 0){//栅形模式
			xReal = (point.x - m_xPadding) / m_ratioX;
			yReal = (point.y - m_yPadding) / m_ratioY;
		}
		else{//方形模式
			CRect mainAreaRect;
			GetDlgItem(IDC_MainArea)->GetClientRect(mainAreaRect);
			int centerX = int((mainAreaRect.Width() - 2 * m_xPadding) / 2 + m_xPadding);//中心坐标x
			int centerY = int((mainAreaRect.Height() - 2 * m_yPadding) / 2 + m_yPadding);//中心坐标y
			xReal = (point.x - centerX) / m_ratioX;
			yReal = (point.y - centerY) / m_ratioY;
		}
		//左上角位置显示x坐标和y坐标
		str.Format(L"X: %5.1f  Y: %5.1f  ", xReal, yReal);
		pDC->SetBkColor(MAINAREA_BG);
		pDC->SetTextColor(RGB(255,255,255));
		pDC->TextOutW(0, 0, str);
	}
	

	CDialogEx::OnMouseMove(nFlags, point);
}


BOOL CSearchAlgDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (GetDlgItem(IDC_MainArea)->GetSafeHwnd() == pMsg->hwnd && pMsg->message == WM_MOUSEMOVE)
	{
		OnMouseMove(0, pMsg->pt);
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}



