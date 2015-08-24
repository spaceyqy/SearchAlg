#pragma once
#include "afxwin.h"


// CfpResultDlg 对话框

class CfpResultDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CfpResultDlg)

public:
	CfpResultDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CfpResultDlg();

// 对话框数据
	enum { IDD = IDD_FPResult };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 航点显示
	CEdit m_fPResult;
};
