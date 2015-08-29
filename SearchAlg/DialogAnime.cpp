// DialogAnime.cpp : 实现文件
//

#include "stdafx.h"
#include "SearchAlg.h"
#include "DialogAnime.h"
#include "afxdialogex.h"


// CDialogAnime 对话框

IMPLEMENT_DYNAMIC(CDialogAnime, CDialogEx)

CDialogAnime::CDialogAnime(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogAnime::IDD, pParent)
	, m_periodMS(1)
	, m_pixelStep(2)
	, m_routeAlpha(90)
{
	
}

CDialogAnime::~CDialogAnime()
{
}

void CDialogAnime::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_Period, m_periodMS);
	DDV_MinMaxInt(pDX, m_periodMS, 0, 10000);
	DDX_Text(pDX, IDC_PixelStep, m_pixelStep);
	DDV_MinMaxInt(pDX, m_pixelStep, 0, 1000);
	DDX_Text(pDX, IDC_RouteAlpha, m_routeAlpha);
	DDV_MinMaxInt(pDX, m_routeAlpha, 0, 255);
}


BEGIN_MESSAGE_MAP(CDialogAnime, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDialogAnime::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SetAreaColorBTN, &CDialogAnime::OnBnClickedSetareacolorbtn)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDialogAnime 消息处理程序


void CDialogAnime::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);//将该界面上的东西放到程序变量中
	CDialogEx::OnOK();
}


void CDialogAnime::OnBnClickedSetareacolorbtn()
{
	// TODO:  在此添加控件通知处理程序代码
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
		m_color = dlg.GetColor();
	//将颜色显示在左边的矩形上
	CWnd *pWnd = GetDlgItem(IDC_ColorDisplay);
	CDC *pDC = pWnd->GetDC();
	CRect rect;
	pWnd->GetClientRect(rect);
	pDC->FillSolidRect(rect,m_color);
}


BOOL CDialogAnime::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CDialogAnime::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码

	CWnd *pWnd = GetDlgItem(IDC_ColorDisplay);
	CDC *pDC = pWnd->GetDC();
	CRect rect;
	pWnd->GetClientRect(rect);

	//先设定刷新，再进行填充才行。否则会覆盖，返回默认值
	pWnd->Invalidate();
	pWnd->UpdateWindow();

	pDC->FillSolidRect(rect, m_color);


	// 不为绘图消息调用 CDialogEx::OnPaint()
}
