// ShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ShowDlg.h"
#include "DiffDialog.h"

// ShowDlg 对话框

IMPLEMENT_DYNAMIC(ShowDlg, CDialog)

ShowDlg::ShowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ShowDlg::IDD, pParent)
{
	m_width=m_height=-1;
	m_cursor_prev.SetPoint(-1,-1);
	m_cursor_prev_old.SetPoint(-1,-1);
	buffer=0;
	m_mutex=CreateMutex(NULL,0,NULL);
	p_mainWnd=NULL;
	m_link[0]=m_link[1]=m_link[2]=NULL;
	m_info.curframe=m_info.contrast=-1;
	m_YUVBuffer1=(char*)align_malloc(1920*1080*3/2);
	m_YUVBuffer2=(char*)align_malloc(1920*1080*3/2);
	flagBuffer=(char*)align_malloc((64+64+62+62)*3);
	diff=new DiffDialog();
	clickCount=0;
	firstFlag=true;
	m_curContext=-1;
}

ShowDlg::~ShowDlg()
{
	if(buffer)
		align_free(buffer);
	if(m_mutex)
		CloseHandle(m_mutex);
	if(m_YUVBuffer1)
		align_free(m_YUVBuffer1);
	if(m_YUVBuffer2)
		align_free(m_YUVBuffer2);
	if(flagBuffer)
		align_free(flagBuffer);
	
	clickCount=-1;
}

void ShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	MoveWindow(140,730,530,180,true);
}


BEGIN_MESSAGE_MAP(ShowDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// ShowDlg 消息处理程序

BOOL ShowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  在此添加额外的初始化
	if(buffer)
	{
		CRect wndRect,cliRect;
		this->GetWindowRect(&wndRect);
		this->GetClientRect(&cliRect);
		this->SetWindowPos(NULL,0,0,m_width+wndRect.Width()-cliRect.Width(),m_height+wndRect.Height()-cliRect.Height(),SWP_NOMOVE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void ShowDlg::SetBitmap(char *bits)
{
	WaitForSingleObject(m_mutex,INFINITE);
	CDC *pDC=GetDC();
	memcpy(buffer,bits,m_height*m_width*4);
	m_cursor_prev.SetPoint(-1,-1);
	StretchDIBits(pDC->GetSafeHdc(),0,0,m_width,m_height,0,0,m_width,m_height,buffer,&m_bmi,DIB_RGB_COLORS,SRCCOPY);
	ReleaseDC(pDC);
	ReleaseMutex(m_mutex);
}

void ShowDlg::OnPaint()
{
	CPaintDC dc(this);
	WaitForSingleObject(m_mutex,INFINITE);
	StretchDIBits(dc.GetSafeHdc(),0,0,m_width,m_height,0,0,m_width,m_height,buffer,&m_bmi,DIB_RGB_COLORS,SRCCOPY);
	ReleaseMutex(m_mutex);
	if(m_link[2]!=NULL)
	{
		if(m_link[2]->GetIsSame())
			m_link[2]->ShowSame();
	}
}

void ShowDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_curContext == 2)//显示不同图像，只有当画面不同时才可以响应鼠标移动
	{
		if(GetIsSame())
			return;
	}
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	pt.x=(pt.x>>6);
	pt.y=(pt.y>>6);
	MousePoint(pt);
	CDialog::OnMouseMove(nFlags, point);
}
void ShowDlg::RefreshCaption()
{
	WaitForSingleObject(m_mutex,INFINITE);
	CString captain;
	if(m_curContext == 0 || m_curContext == 1)
		captain.Format(L"File :%d  ,  Frame :%d  ,  Coordinate :( %d , %d ) ,  PATHS :%s",m_info.contextid+1,m_info.curframe,m_cursor_prev.x,m_cursor_prev.y,m_info.filename);
	else
		captain.Format(L" Frame :%d  ,  Coordinate :(%d,%d) White means different",m_info.curframe,m_cursor_prev.x,m_cursor_prev.y);
	this->SetWindowTextW(captain);
	this->GetParent()->SetWindowTextW(captain);
	ReleaseMutex(m_mutex);
}
BOOL ShowDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN||pMsg->message==WM_SYSKEYDOWN)
	{
		if(S_OK==OnKeyDown(pMsg->wParam,pMsg->lParam))
			return 1;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void ShowDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_curContext == 2)//显示不同图像，只有当画面不同时才可以响应鼠标移动
	{
		if(GetIsSame())
			return;
	}
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	pt.x=(pt.x>>6);
	pt.y=(pt.y>>6);
	if(clickCount==0 )//第一次点击
	{
		diff->Create(IDD_DIFF,NULL);
		diff->SetCurContext(m_curContext);
	}
	else
		diff->ShowWindow(SW_HIDE);
	diff->SetWH(m_width,m_height);
	diff->SetBuffer((unsigned char *)m_YUVBuffer1,(unsigned char *)m_YUVBuffer2,pt.x,pt.y);  
	diff->ShowWindow(SW_SHOW); 
	diff->SetCurFrame(m_info.curframe);
	diff->ini();
	MosePoint_old(pt);
	firstFlag=false;
	CDialog::OnLButtonDown(nFlags, point);
}


void ShowDlg::OnClose()
{

	CDialog::OnClose();
}
