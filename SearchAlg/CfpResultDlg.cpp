// CfpResultDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SearchAlg.h"
#include "CfpResultDlg.h"
#include "afxdialogex.h"


// CfpResultDlg 对话框

IMPLEMENT_DYNAMIC(CfpResultDlg, CDialogEx)

CfpResultDlg::CfpResultDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CfpResultDlg::IDD, pParent)
{

}

CfpResultDlg::~CfpResultDlg()
{
}

void CfpResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_fPResult, m_fPResult);
}


BEGIN_MESSAGE_MAP(CfpResultDlg, CDialogEx)
END_MESSAGE_MAP()


// CfpResultDlg 消息处理程序
