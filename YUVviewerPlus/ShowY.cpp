// ShowY.cpp : 实现文件
//

#include "stdafx.h"
#include "YUVviewerPlus.h"
#include "ShowY.h"


// ShowY 对话框

IMPLEMENT_DYNAMIC(ShowY, CDialog)

ShowY::ShowY(CWnd* pParent /*=NULL*/)
	: CDialog(ShowY::IDD, pParent)
{
	yBuffer1=(char*)align_malloc(4*4);
	yBuffer2=(char*)align_malloc(4*4);
	uBuffer1=(char*)align_malloc(2*2);
	uBuffer2=(char*)align_malloc(2*2);
	vBuffer1=(char*)align_malloc(2*2);
	vBuffer2=(char*)align_malloc(2*2);
	m_width=m_height=-1;
	index=pox=poy=-1;
	curFrame=-1;
	
}

ShowY::~ShowY()
{
	if(yBuffer1)
		align_free(yBuffer1);
	if(yBuffer2)
		align_free(yBuffer2);
	if(uBuffer1)
		align_free(uBuffer1);
	if(uBuffer2)
		align_free(uBuffer2);
	if(vBuffer1)
		align_free(vBuffer1);
	if(vBuffer2)
		align_free(vBuffer2);
}

void ShowY::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	CButton* radio=(CButton*)GetDlgItem(IDC_RADIO1);
	radio->SetCheck(1);
	counter=0;
}


BEGIN_MESSAGE_MAP(ShowY, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_RADIO1, &ShowY::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &ShowY::OnBnClickedRadio2)
END_MESSAGE_MAP()


// ShowY 消息处理程序

void ShowY::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// 不为绘图消息调用 CDialog::OnPaint()
	CRect rect;
	GetClientRect(rect);
    dc.FillSolidRect(rect,RGB(255,255,255));
	int step=35,x0=40,x1=x0+step*4,y0=40,y1=y0+step*4,x2=300,x3=400;
	int i,j,k,l;
	///////////Y的具体值
	for(i=x0,j=0;j<=4;i+=step,j++)
	{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(x0,i);
			dc.LineTo(x1,i);
	}
	for(i=x0,j=0;j<=4;i+=step,j++)
	{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(i,y0);
			dc.LineTo(i,y1);
	}
	if(m_curContext == 2)
	{
	for(i=x0,j=0;j<=4;i+=step,j++)
	{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(x0+200,i);
			dc.LineTo(x1+200,i);
	}
	for(i=x0+200,j=0;j<=4;i+=step,j++)
	{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(i,y0);
			dc.LineTo(i,y1);
	}
	}
	///////////U的具体值
	int num=160,num1=69;//下偏移
	for(i=x0+num,j=0;j<=2;i+=step,j++)
	{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(x0,i);
			dc.LineTo(x1-num1,i);
	}
	for(i=x0,j=0;j<=2;i+=step,j++)
	{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(i,y0+num);
			dc.LineTo(i,y1+num-num1);
	}
	if(m_curContext == 2)
	{
	for(i=x0+num,j=0;j<=2;i+=step,j++)
	{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(x0+200,i);
			dc.LineTo(x1+200-num1,i);
	}
	for(i=x0+200,j=0;j<=2;i+=step,j++)
	{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(i,y0+num);
			dc.LineTo(i,y1+num-num1);
	}
	}
	///////////V的具体值
	num=250;//下移量
	for(i=x0+num,j=0;j<=2;i+=step,j++)
	{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(x0,i);
			dc.LineTo(x1-num1,i);
	}
	for(i=x0,j=0;j<=2;i+=step,j++)
	{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(i,y0+num);
			dc.LineTo(i,y1+num-num1);
	}
	if(m_curContext == 2)
	{
	for(i=x0+num,j=0;j<=2;i+=step,j++)
	{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(x0+200,i);
			dc.LineTo(x1+200-num1,i);
	}
	for(i=x0+200,j=0;j<=2;i+=step,j++)
	{
			CPen   pen   (PS_SOLID,   1,   RGB   (0,   0,   0));         
			dc.SelectObject(&pen);
			dc.MoveTo(i,y0+num);
			dc.LineTo(i,y1+num-num1);
	}
	}

	CString ind;
	ind.Format(_T("Z-index=%d"),index);
	dc.TextOutW(150,10,ind);
	ind.Format(_T("(%d,%d)"),pox/4,poy/4);
	dc.TextOutW(90,10,ind);

	dc.TextOutW(10,90,_T("Y1"));
	dc.TextOutW(10,220,_T("U1"));
	dc.TextOutW(10,310,_T("V1"));
	if(m_curContext == 2)
	{
		dc.TextOutW(208,90,_T("Y2"));
		dc.TextOutW(208,220,_T("U2"));
		dc.TextOutW(208,310,_T("V2"));
	}
	int fontnum=8;//字相对于表格的偏移
	///Y的值
	for(i=y0+fontnum,k=0;k<4;i+=step,k++)
		for(j=x0+fontnum,l=0;l<4;j+=step,l++)
		{
			CString str; 
		    int num;
			num = (int)(unsigned char)(yBuffer1[k*4+l]);

			if(yBuffer1[k*4+l] != yBuffer2[k*4+l])
			{
				 dc.SetTextColor(RGB(255,0,0));
			}
			else
			{
				dc.SetTextColor(RGB(0,0,0)); 
			}
			if(counter%2==0)
				str.Format(_T("%d"),num);
			else
				str.Format(_T("%x"),num);
			dc.TextOutW(j,i,str);
			if(m_curContext == 2)
			{
			num = (int)(unsigned char)yBuffer2[k*4+l];
			if(counter%2==0)
				str.Format(_T("%d"),num);
			else
				str.Format(_T("%x"),num);;
			dc.TextOutW(j+200,i,str);
			}
		}
	///U的值
	num=160;
	for(i=y0+fontnum+num,k=0;k<2;i+=step,k++)
		for(j=x0+fontnum,l=0;l<2;j+=step,l++)
		{
			CString str; 
		    int num;
			num = (int)(unsigned char)(uBuffer1[k*2+l]);
			if(uBuffer1[k*2+l] != uBuffer2[k*2+l])
			{
				 dc.SetTextColor(RGB(255,0,0));
			}
			else
			{
				dc.SetTextColor(RGB(0,0,0)); 
			}
			if(counter%2==0)
				str.Format(_T("%d"),num);
			else
				str.Format(_T("%x"),num);
			dc.TextOutW(j,i,str);
			if(m_curContext == 2)
			{
			num = (int)(unsigned char)uBuffer2[k*2+l];
			if(counter%2==0)
				str.Format(_T("%d"),num);
			else
				str.Format(_T("%x"),num);
			dc.TextOutW(j+200,i,str);	
			}
		}
	///V的值
	num=250;
	for(i=y0+fontnum+num,k=0;k<2;i+=step,k++)
		for(j=x0+fontnum,l=0;l<2;j+=step,l++)
		{
			CString str; 
		    int num;
			num = (int)(unsigned char)(vBuffer1[k*2+l]);
			if(vBuffer1[k*2+l] != vBuffer2[k*2+l])
			{
				 dc.SetTextColor(RGB(255,0,0));
			}
			else
			{
				dc.SetTextColor(RGB(0,0,0)); 
			}
			if(counter%2==0)
				str.Format(_T("%d"),num);
			else
				str.Format(_T("%x"),num);
			dc.TextOutW(j,i,str);
			if(m_curContext == 2)
			{
			num = (int)(unsigned char)vBuffer2[k*2+l];
			if(counter%2==0)
				str.Format(_T("%d"),num);
			else
				str.Format(_T("%x"),num);
			dc.TextOutW(j+200,i,str);	
			}
		}
		RefreshCaption();
}



void ShowY::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	counter=0;
	Invalidate();
}

void ShowY::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	counter=1;
	Invalidate();
}
