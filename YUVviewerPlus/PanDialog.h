#pragma once
//#include "YUVmanager.h"
#include "YUVviewerPlus.h"
// PanDialog �Ի���

class PanDialog : public CDialog
{
	DECLARE_DYNAMIC(PanDialog)

public:
	PanDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~PanDialog();
	void myClose();
	int m_curContext;
	void SetCurContext(int n)
	{
		m_curContext=n;
	}
	
// �Ի�������
	enum { IDD = IDD_DIALOG_PAN };

private:
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
