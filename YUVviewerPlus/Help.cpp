// Help.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "YUVviewerPlus.h"
#include "Help.h"


// Help �Ի���

IMPLEMENT_DYNAMIC(Help, CDialog)

Help::Help(CWnd* pParent /*=NULL*/)
	: CDialog(Help::IDD, pParent)
{

}

Help::~Help()
{
}

void Help::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	MoveWindow(100,200,300,180,true);
}


BEGIN_MESSAGE_MAP(Help, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// Help ��Ϣ�������

void Help::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
	CRect rect;
	GetClientRect(rect);
    dc.FillSolidRect(rect,RGB(255,255,255));
	int x=10,y=10,step=30;
	dc.TextOutW(x,y,_T("EXC :�ر����д���"));
	dc.TextOutW(x,y+step,_T("   r   :  ����֡"));
	dc.TextOutW(x,y+2*step,_T("�ո�:����/��ͣ"));
	dc.TextOutW(x,y+3*step,_T(" ��  : ��һ֡"));
	dc.TextOutW(x,y+4*step,_T(" ��  : ǰһ֡"));
//	dc.TextOutW(13*x+10,y+4*step,_T("yingmingfan@qq.com"));
}
