#pragma once
#include "afxcmn.h"


// OptionDlg 对话框

class OptionDlg : public CDialog
{
	DECLARE_DYNAMIC(OptionDlg)

public:
	OptionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~OptionDlg();
	virtual BOOL OnInitDialog();
// 对话框数据
	enum { IDD = IDD_DIALOG_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;
	int colCount;
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnLvnItemchangedList3(NMHDR *pNMHDR, LRESULT *pResult);
};
