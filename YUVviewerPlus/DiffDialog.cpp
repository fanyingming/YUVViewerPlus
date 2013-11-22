// DiffDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "YUVviewerPlus.h"
#include "DiffDialog.h"
#include "ShowY.h"

// DiffDialog 对话框

IMPLEMENT_DYNAMIC(DiffDialog, CDialog)

DiffDialog::DiffDialog(CWnd* pParent /*=NULL*/)
	: CDialog(DiffDialog::IDD, pParent)

{
	x0=35,y0=10,step=7,x1=step*64+x0,y1=step*64+y0;
	m_cursor_prev.SetPoint(-999,-999);
	YBuffer1=(char*)align_malloc(64*64);
	YBuffer2=(char*)align_malloc(64*64);
	UBuffer1=(char*)align_malloc(32*32);
	UBuffer2=(char*)align_malloc(32*32);
	VBuffer1=(char*)align_malloc(32*32);
	VBuffer2=(char*)align_malloc(32*32);
	m_width=m_height=-1;
	clickCount=0;
	diff=new ShowY();
	painFlag=false;
	num=480;//平移,U表格的左起始画线点相对于Y表格左起始画线点向右偏移
	num1=256;//U表格的右起始画线点相对于Y表格右起始画线点向右偏移
	num3=224;
	num4=240;
	curFrame=-1;
    ini();
}

DiffDialog::~DiffDialog()
{
	if(YBuffer1)
		align_free(YBuffer1);
	if(YBuffer2)
		align_free(YBuffer2);
	if(UBuffer1)
		align_free(UBuffer1);
	if(UBuffer2)
		align_free(UBuffer2);
	if(VBuffer1)
		align_free(VBuffer1);
	if(VBuffer2)
		align_free(VBuffer2);
	clickCount=-1;
}

void DiffDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	SetWindowPos(NULL,0,0,770,520,SWP_NOMOVE | SWP_NOZORDER);
//	MoveWindow(0,0,770,520,true);
}


BEGIN_MESSAGE_MAP(DiffDialog, CDialog)
//	ON_EN_CHANGE(IDC_EDIT1, &DiffDialog::OnEnChangeEdit1)
ON_WM_PAINT()
ON_WM_VSCROLL()
ON_WM_ERASEBKGND()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// DiffDialog 消息处理程序


void DiffDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	CRect rect;
	GetClientRect(rect);
    dc.FillSolidRect(rect,RGB(255,255,255));
	int i,j,m,n;
	///Y表格
	for(i=y0,j=0;j<=64;i+=step,j++)
	{
		if(j%4==0 )
		{   
			CPen   pen   (PS_SOLID,   2,   RGB   (0,   0,   0)); 
			dc.SelectObject(&pen); 
			dc.MoveTo(x0-1,i);
			dc.LineTo(x1,i);
		}
		else
		{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(x0,i);
			dc.LineTo(x1,i);
		}
	}
	for(i=x0,j=0;j<=64;i+=step,j++)
	{
		 if(j%4==0 )
		{  
			CPen   pen   (PS_SOLID,   2,   RGB   (0,   0,   0)); 
			dc.SelectObject(&pen); 
			dc.MoveTo(i,y0-1);
			dc.LineTo(i,y1);
		}
		else
		{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(i,y0);
			dc.LineTo(i,y1);
		}
	}
	///U表格

	for(i=y0,j=0;j<=32;i+=step,j++)
	{
		if(j%2==0 )
		{
			CPen   pen   (PS_SOLID,   2,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen); 
			dc.MoveTo(x0+num-1,i);
			dc.LineTo(x1+num1,i);
		}
		else
		{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(x0+num,i);
			dc.LineTo(x1+num1,i);
		}
	}
	for(i=x0+num,j=0;j<=32;i+=step,j++)
	{
		if(j%2==0 )
		{
			CPen   pen   (PS_SOLID,   2,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen); 
			dc.MoveTo(i,y0-1);
			dc.LineTo(i,y1-num3);
		}
		else
		{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(i,y0);
			dc.LineTo(i,y1-num3);
		}
	}
	////V表格
	
	for(i=y0+num4,j=0;j<=32;i+=step,j++)
	{
		if(j%2==0 )
		{
			CPen   pen   (PS_SOLID,   2,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen); 
			dc.MoveTo(x0+num-1,i);
			dc.LineTo(x1+num1,i);
		}
		else
		{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(x0+num,i);
			dc.LineTo(x1+num1,i);
		}
	}
	
	for(i=x0+num,j=0;j<=32;i+=step,j++)
	{
		if(j%2==0 )
		{
			CPen   pen   (PS_SOLID,   2,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen); 
			dc.MoveTo(i,y0+num4-1);
			dc.LineTo(i,y1-num3+num4);
		}
		else
		{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(i,y0+num4);
			dc.LineTo(i,y1-num3+num4);
		}
	}
	/////////////////
/*
	CFont   font;   
	LOGFONT   lf;   
	memset(&lf,   0,   sizeof(LOGFONT));   
	lf.lfHeight   =   5;         //字体的高   
	lf.lfWidth    =  3;         //字体宽
	font.CreateFontIndirect(&lf);
	dc.SelectObject(&font);
	
	CString symbol;
	symbol=_T("■");
	*/
	////Y
	int diffOffSet=1;
	for(i=y0+diffOffSet,j=0;j<=63;i+=step,j++)
		for(m=x0+diffOffSet,n=0;n<=63;m+=step,n++)
		{
			if(YBuffer1[j*64+n] != YBuffer2[j*64+n])
			{
				if(j%4==3 && n%4==3)
					dc.FillSolidRect(CRect(m,i,m+step-diffOffSet-1,i+step-diffOffSet-1),RGB(150,150,150));
				else if(j%4==3)
					dc.FillSolidRect(CRect(m,i,m+step-diffOffSet,i+step-diffOffSet-1),RGB(150,150,150));
				else if(n%4==3)
					dc.FillSolidRect(CRect(m,i,m+step-diffOffSet-1,i+step-diffOffSet),RGB(150,150,150));
				else
				    dc.FillSolidRect(CRect(m,i,m+step-diffOffSet,i+step-diffOffSet),RGB(150,150,150));
			
			}
		}
	////U
	for(i=y0+diffOffSet,j=0;j<=31;i+=step,j++)
		for(m=x0+diffOffSet+num,n=0;n<=31;m+=step,n++)
		{
			if(UBuffer1[j*32+n] != UBuffer2[j*32+n])
			{
				if(j%2==1 && n%2==1)
					dc.FillSolidRect(CRect(m,i,m+step-diffOffSet-1,i+step-diffOffSet-1),RGB(150,150,150));
				else if(j%2==1)
					dc.FillSolidRect(CRect(m,i,m+step-diffOffSet,i+step-diffOffSet-1),RGB(150,150,150));
				else if(n%2==1)
					dc.FillSolidRect(CRect(m,i,m+step-diffOffSet-1,i+step-diffOffSet),RGB(150,150,150));
				else
				    dc.FillSolidRect(CRect(m,i,m+step-diffOffSet,i+step-diffOffSet),RGB(150,150,150));
			}
		}
	////V
	for(i=y0+diffOffSet+num4,j=0;j<=31;i+=step,j++)
		for(m=x0+diffOffSet+num,n=0;n<=31;m+=step,n++)
		{
			
			if(VBuffer1[j*32+n] != VBuffer2[j*32+n])
			{
				if(j%2==1 && n%2==1)
					dc.FillSolidRect(CRect(m,i,m+step-diffOffSet-1,i+step-diffOffSet-1),RGB(150,150,150));
				else if(j%2==1)
					dc.FillSolidRect(CRect(m,i,m+step-diffOffSet,i+step-diffOffSet-1),RGB(150,150,150));
				else if(n%2==1)
					dc.FillSolidRect(CRect(m,i,m+step-diffOffSet-1,i+step-diffOffSet),RGB(150,150,150));
				else
				    dc.FillSolidRect(CRect(m,i,m+step-diffOffSet,i+step-diffOffSet),RGB(150,150,150));
			}
		}
	CFont   font1;   
	LOGFONT   lf1;   
	memset(&lf1,   0,   sizeof(LOGFONT));   
	lf1.lfHeight   =   20;         //字体的高   
	lf1.lfWidth    =  9;         //字体宽
	font1.CreateFontIndirect(&lf1);
	dc.SelectObject(&font1);
	dc.FillSolidRect(CRect(5,y0+10,5+9,y0+10+20),RGB(255,255,255));
	dc.TextOutW(5,y0+10,_T("Y"));
	dc.TextOutW(x0+num-20,y0+10,_T("U"));
	dc.TextOutW(x0+num-20,y0+270,_T("V"));
	painFlag=true;
}



BOOL DiffDialog::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return CDialog::OnEraseBkgnd(pDC);
	
}

void DiffDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(painFlag)
	{
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		//Y的表格范围
		if(pt.x >= x0 && pt.x < x0+step*64 && pt.y>=y0 && pt.y < y0 + step*64 )
		{
			pt.x=((pt.x-x0)/(step*4));
			pt.y=((pt.y-y0)/(step*4));
			MousePoint(pt);
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}

void DiffDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	if(pt.x >= x0 && pt.x <= x0+step*64 && pt.y>=y0 && pt.y <= y0 + step*64 )
	{
		int x,y;
		pt.x=x=((pt.x-x0)/(step*4));
		pt.y=y=((pt.y-y0)/(step*4));
		if(clickCount==0)//第一次点击
		{
			diff->Create(IDD_SHOWY,NULL); 
			diff->SetCurContext(m_curContext);
		}
		else
		{
			diff->ShowWindow(SW_HIDE);
		}
		diff->SetWH(m_width,m_height);
		diff->SetData(pos[y][x],x*4,y*4);
		diff->SetBuffer((unsigned char*)YBuffer1,(unsigned char*)YBuffer2,(unsigned char*)UBuffer1,(unsigned char*)UBuffer2,(unsigned char*)VBuffer1,(unsigned char*)VBuffer2);	
		diff->ShowWindow(SW_SHOW);  
		diff->SetCurFrame(curFrame);
		MousePoint_old(pt);
		clickCount++;
	}
	CDialog::OnLButtonDown(nFlags, point);
}
