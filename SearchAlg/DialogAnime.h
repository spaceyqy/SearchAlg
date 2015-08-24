#pragma once


// CDialogAnime 对话框

class CDialogAnime : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogAnime)

public:
	CDialogAnime(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogAnime();

// 对话框数据
	enum { IDD = IDD_SetAnime };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 绘制周期
	int m_periodMS;
	// 绘制步进
	int m_pixelStep;
	afx_msg void OnBnClickedOk();
	// 路径透明度，0-255表明全透-不透
	int m_routeAlpha;
};
