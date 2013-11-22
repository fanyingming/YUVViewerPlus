// PanDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "YUVviewerPlusDlg.h"
#include "PanDialog.h"


// PanDialog �Ի���

IMPLEMENT_DYNAMIC(PanDialog, CDialog)

PanDialog::PanDialog(CWnd* pParent /*=NULL*/)
	: CDialog(PanDialog::IDD, pParent)
{

}

PanDialog::~PanDialog()
{
}

void PanDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PanDialog, CDialog)
	ON_MESSAGE_VOID(WM_CLOSE,myClose)
END_MESSAGE_MAP()


// PanDialog ��Ϣ�������
void PanDialog::myClose()
{
	HWND hWnd=::FindWindow(NULL,_T("YUVviewerPlus")); 
	CYUVviewerPlusDlg* pWnd= (CYUVviewerPlusDlg*)FromHandle(hWnd);
	pWnd->closeWindow(m_curContext);

	this->OnClose();
}