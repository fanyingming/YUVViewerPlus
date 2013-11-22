// Help.cpp : 实现文件
//

#include "stdafx.h"
#include "YUVviewerPlus.h"
#include "Help.h"


// Help 对话框

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


// Help 消息处理程序

void Help::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	CRect rect;
	GetClientRect(rect);
    dc.FillSolidRect(rect,RGB(255,255,255));
	int x=10,y=10,step=30;
	dc.TextOutW(x,y,_T("EXC :关闭所有窗口"));
	dc.TextOutW(x,y+step,_T("   r   :  重置帧"));
	dc.TextOutW(x,y+2*step,_T("空格:播放/暂停"));
	dc.TextOutW(x,y+3*step,_T(" →  : 下一帧"));
	dc.TextOutW(x,y+4*step,_T(" ←  : 前一帧"));
//	dc.TextOutW(13*x+10,y+4*step,_T("yingmingfan@qq.com"));
}
