#pragma once
//#include "YUVmanager.h"
#include "YUVviewerPlus.h"
// PanDialog 对话框

class PanDialog : public CDialog
{
	DECLARE_DYNAMIC(PanDialog)

public:
	PanDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~PanDialog();
	void myClose();
	int m_curContext;
	void SetCurContext(int n)
	{
		m_curContext=n;
	}
	
// 对话框数据
	enum { IDD = IDD_DIALOG_PAN };

private:
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
