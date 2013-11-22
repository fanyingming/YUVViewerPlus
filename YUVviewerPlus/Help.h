#pragma once


// Help 对话框

class Help : public CDialog
{
	DECLARE_DYNAMIC(Help)

public:
	Help(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Help();

// 对话框数据
	enum { IDD = IDD_HELP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};
