#pragma once
#include "alignmemory.h"
#include "ShowY.h"
#include "Zindex.h"
// DiffDialog 对话框
#define PIC_SIZE ((m_width*m_height*3)>>1)

class DiffDialog : public CDialog
{
	DECLARE_DYNAMIC(DiffDialog)

public:
	DiffDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DiffDialog();
	char *YBuffer1,*YBuffer2,*UBuffer1,*UBuffer2,*VBuffer1,*VBuffer2;
	int m_width,m_height;
	int x0,y0,step,x1,y1;
	int clickCount;
	ShowY *diff;
	CPoint m_cursor_prev,m_cursor_prev_old,m_cursor_prev_flag;
	bool painFlag;
	int num;//平移
	int num1,num3;
	int num4;
	int edgeCount;
	bool Edge_Recv,Edge_Flag;
	int m_curContext;
	int curFrame;
	void SetCurFrame(int n)
	{
		curFrame=n;
	}
	void SetCurContext(int n)
	{
		m_curContext=n;
	}
	void SetWH(int width,int height)
	{
		m_height=height;
		m_width=width;
	}
	//将64*64的区域中YUV分别截取出来，保存到YBuffer，UBuffer，VBuffer中
	void SetBuffer(unsigned char * m_YUVBuffer1,unsigned char * m_YUVBuffer2,int pox,int poy)
	{
		unsigned char* y1 = (unsigned char*)m_YUVBuffer1;
		unsigned char* u1 = y1 + m_width * m_height;
		unsigned char* v1 = u1 + m_width * m_height / 4;
		unsigned char* y2 = (unsigned char*)m_YUVBuffer2;
		unsigned char* u2 = y2 + m_width * m_height;
		unsigned char* v2 = u2 + m_width * m_height / 4;
		int count=0,countuv=0;
		int x=(pox<<6),y=(poy<<6);
		for ( int i = 0; i < m_height; i++ )
		{
			for ( int j = 0; j < m_width; j++ )
			{
				if(j >= x && j <= x+63 && i >= y && i <= y+63)
				{		
					YBuffer1[count]=y1[j];
					YBuffer2[count]=y2[j];
					if(i%2==0 &&j%2==0)
					{
						UBuffer1[countuv]=u1[j / 2];
						UBuffer2[countuv]=u2[j / 2];
						VBuffer1[countuv]=v1[j / 2];
						VBuffer2[countuv]=v2[j / 2];
						countuv++;
					}
					count++;
					if(j == m_width-1)//右边界
					{
						for(int k = m_width-x;k<=63;k++,count++)
						{
							YBuffer1[count]=0;
							YBuffer2[count]=0;
							if(i%2==0 &&j%2==0)
							{
								UBuffer1[countuv]=u1[j / 2];
								UBuffer2[countuv]=u2[j / 2];
								VBuffer1[countuv]=v1[j / 2];
								VBuffer2[countuv]=v2[j / 2];
								countuv++;
							}
						}
					}
				}
			}
			y1 += m_width;
			y2 += m_width;
			if ( (i & 0x1) != 0 )//if(i%2==1)
			{
				u1 += m_width / 2;
				v1 += m_width / 2;
				u2 += m_width / 2;
				v2 += m_width / 2;
			}
		}
		//下边界
		if( count < 64*64 )
		{
			for(;count<64*64;count++)
			{
				YBuffer1[count]=0;
				YBuffer2[count]=0;
			}
		}
		//uv下边界
		if(countuv<32*32)
		{
			for( ;countuv<32*32;countuv++)
			{			
				UBuffer1[countuv]=0;
				UBuffer2[countuv]=0;
				VBuffer1[countuv]=0;
				VBuffer2[countuv]=0;	
			}
		}

}
	void MousePoint(CPoint pt,bool refresh=1)
	{
		if(pt!=m_cursor_prev)
		{
			DrawBlockEdge(m_cursor_prev);
			m_cursor_prev=pt;
			DrawBlockEdge(m_cursor_prev);
			if(refresh)
				RefreshCaption();
		}
	}
	void MousePoint_old(CPoint pt,bool refresh=1)
	{
		if(pt!=m_cursor_prev_flag)
		{
			if(clickCount == 0)//第一次点击
			{
				m_cursor_prev_flag=m_cursor_prev_old=pt;
				Edge_Recv=false;
				edgeCount++;
			}
			else
			{
				m_cursor_prev_old=m_cursor_prev_flag;
				m_cursor_prev_flag=pt;
				Edge_Recv=true;
				edgeCount++;
			}

			DrawBlockEdge(pt);
			if(refresh)
				RefreshCaption();
			
		}
	}
	
	void DrawBlockEdge(CPoint pt)
	{
		CClientDC dc(this);
		int x,y;
		if(Edge_Recv)
		{
			x=m_cursor_prev_old.x*step*4+x0;
			y=m_cursor_prev_old.y*step*4+y0;
			CPen   pen   (PS_SOLID,   2,   RGB   (0,  0,   0));    
			dc.SelectObject(&pen); 
			////Y
			dc.MoveTo(x,y);
			dc.LineTo(x+step*4,y);
			dc.MoveTo(x,y+step*4);
			dc.LineTo(x+step*4,y+step*4);
			dc.MoveTo(x,y);
			dc.LineTo(x,y+step*4);
			dc.MoveTo(x+step*4,y);
			dc.LineTo(x+step*4,y+step*4);
			///U
			dc.MoveTo(x0+num+m_cursor_prev_old.x*step*2,y0+m_cursor_prev_old.y*step*2);
			dc.LineTo(x0+num+m_cursor_prev_old.x*step*2+step*2,y0+m_cursor_prev_old.y*step*2);
			dc.MoveTo(x0+num+m_cursor_prev_old.x*step*2,y0+m_cursor_prev_old.y*step*2+step*2);
			dc.LineTo(x0+num+m_cursor_prev_old.x*step*2+step*2,y0+m_cursor_prev_old.y*step*2+step*2);
			dc.MoveTo(x0+num+m_cursor_prev_old.x*step*2,y0+m_cursor_prev_old.y*step*2);
			dc.LineTo(x0+num+m_cursor_prev_old.x*step*2,y0+m_cursor_prev_old.y*step*2+step*2);
			dc.MoveTo(x0+num+m_cursor_prev_old.x*step*2+step*2,y0+m_cursor_prev_old.y*step*2);
			dc.LineTo(x0+num+m_cursor_prev_old.x*step*2+step*2,y0+m_cursor_prev_old.y*step*2+step*2);
			///V
			dc.MoveTo(x0+num+m_cursor_prev_old.x*step*2,y0+m_cursor_prev_old.y*step*2+num4);
			dc.LineTo(x0+num+m_cursor_prev_old.x*step*2+step*2,y0+m_cursor_prev_old.y*step*2+num4);
			dc.MoveTo(x0+num+m_cursor_prev_old.x*step*2,y0+m_cursor_prev_old.y*step*2+step*2+num4);
			dc.LineTo(x0+num+m_cursor_prev_old.x*step*2+step*2,y0+m_cursor_prev_old.y*step*2+step*2+num4);
			dc.MoveTo(x0+num+m_cursor_prev_old.x*step*2,y0+m_cursor_prev_old.y*step*2+num4);
			dc.LineTo(x0+num+m_cursor_prev_old.x*step*2,y0+m_cursor_prev_old.y*step*2+step*2+num4);
			dc.MoveTo(x0+num+m_cursor_prev_old.x*step*2+step*2,y0+m_cursor_prev_old.y*step*2+num4);
			dc.LineTo(x0+num+m_cursor_prev_old.x*step*2+step*2,y0+m_cursor_prev_old.y*step*2+step*2+num4);
		}
		if(Edge_Flag)
		{
			x=m_cursor_prev_flag.x*step*4+x0;
			y=m_cursor_prev_flag.y*step*4+y0;
			CRect rect;
			rect.SetRect(x,y,x+step*4,y+step*4);
			CPen   pen   (PS_SOLID,   2,   RGB   (0,  255,   0));   
			dc.SelectObject(&pen); 
			////Y
			dc.MoveTo(x,y);
			dc.LineTo(x+step*4,y);
			dc.MoveTo(x,y+step*4);
			dc.LineTo(x+step*4,y+step*4);
			dc.MoveTo(x,y);
			dc.LineTo(x,y+step*4);
			dc.MoveTo(x+step*4,y);
			dc.LineTo(x+step*4,y+step*4);
			///U
			dc.MoveTo(x0+num+m_cursor_prev_flag.x*step*2,y0+m_cursor_prev_flag.y*step*2);
			dc.LineTo(x0+num+m_cursor_prev_flag.x*step*2+step*2,y0+m_cursor_prev_flag.y*step*2);
			dc.MoveTo(x0+num+m_cursor_prev_flag.x*step*2,y0+m_cursor_prev_flag.y*step*2+step*2);
			dc.LineTo(x0+num+m_cursor_prev_flag.x*step*2+step*2,y0+m_cursor_prev_flag.y*step*2+step*2);
			dc.MoveTo(x0+num+m_cursor_prev_flag.x*step*2,y0+m_cursor_prev_flag.y*step*2);
			dc.LineTo(x0+num+m_cursor_prev_flag.x*step*2,y0+m_cursor_prev_flag.y*step*2+step*2);
			dc.MoveTo(x0+num+m_cursor_prev_flag.x*step*2+step*2,y0+m_cursor_prev_flag.y*step*2);
			dc.LineTo(x0+num+m_cursor_prev_flag.x*step*2+step*2,y0+m_cursor_prev_flag.y*step*2+step*2);
			///V
			dc.MoveTo(x0+num+m_cursor_prev_flag.x*step*2,y0+m_cursor_prev_flag.y*step*2+num4);
			dc.LineTo(x0+num+m_cursor_prev_flag.x*step*2+step*2,y0+m_cursor_prev_flag.y*step*2+num4);
			dc.MoveTo(x0+num+m_cursor_prev_flag.x*step*2,y0+m_cursor_prev_flag.y*step*2+step*2+num4);
			dc.LineTo(x0+num+m_cursor_prev_flag.x*step*2+step*2,y0+m_cursor_prev_flag.y*step*2+step*2+num4);
			dc.MoveTo(x0+num+m_cursor_prev_flag.x*step*2,y0+m_cursor_prev_flag.y*step*2+num4);
			dc.LineTo(x0+num+m_cursor_prev_flag.x*step*2,y0+m_cursor_prev_flag.y*step*2+step*2+num4);
			dc.MoveTo(x0+num+m_cursor_prev_flag.x*step*2+step*2,y0+m_cursor_prev_flag.y*step*2+num4);
			dc.LineTo(x0+num+m_cursor_prev_flag.x*step*2+step*2,y0+m_cursor_prev_flag.y*step*2+step*2+num4);
		}
		x=pt.x*step*4+x0;
		y=pt.y*step*4+y0;
		if(edgeCount%2==0)
		{
			CPen   pen   (PS_SOLID,   2,   RGB   (0,  0,   0));    
			dc.SelectObject(&pen); 
			////Y
			dc.MoveTo(x,y);
			dc.LineTo(x+step*4,y);
			dc.MoveTo(x,y+step*4);
			dc.LineTo(x+step*4,y+step*4);
			dc.MoveTo(x,y);
			dc.LineTo(x,y+step*4);
			dc.MoveTo(x+step*4,y);
			dc.LineTo(x+step*4,y+step*4);
			///U
			dc.MoveTo(x0+num+pt.x*step*2,y0+pt.y*step*2);
			dc.LineTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2);
			dc.MoveTo(x0+num+pt.x*step*2,y0+pt.y*step*2+step*2);
			dc.LineTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2+step*2);
			dc.MoveTo(x0+num+pt.x*step*2,y0+pt.y*step*2);
			dc.LineTo(x0+num+pt.x*step*2,y0+pt.y*step*2+step*2);
			dc.MoveTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2);
			dc.LineTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2+step*2);
			///V
			dc.MoveTo(x0+num+pt.x*step*2,y0+pt.y*step*2+num4);
			dc.LineTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2+num4);
			dc.MoveTo(x0+num+pt.x*step*2,y0+pt.y*step*2+step*2+num4);
			dc.LineTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2+step*2+num4);
			dc.MoveTo(x0+num+pt.x*step*2,y0+pt.y*step*2+num4);
			dc.LineTo(x0+num+pt.x*step*2,y0+pt.y*step*2+step*2+num4);
			dc.MoveTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2+num4);
			dc.LineTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2+step*2+num4);
		}
		else
		{
			CPen   pen   (PS_SOLID,   2,   RGB   (200,   32,   149));
			dc.SelectObject(&pen); 
			////Y
			dc.MoveTo(x,y);
			dc.LineTo(x+step*4,y);
			dc.MoveTo(x,y+step*4);
			dc.LineTo(x+step*4,y+step*4);
			dc.MoveTo(x,y);
			dc.LineTo(x,y+step*4);
			dc.MoveTo(x+step*4,y);
			dc.LineTo(x+step*4,y+step*4);
			///U
			dc.MoveTo(x0+num+pt.x*step*2,y0+pt.y*step*2);
			dc.LineTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2);
			dc.MoveTo(x0+num+pt.x*step*2,y0+pt.y*step*2+step*2);
			dc.LineTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2+step*2);
			dc.MoveTo(x0+num+pt.x*step*2,y0+pt.y*step*2);
			dc.LineTo(x0+num+pt.x*step*2,y0+pt.y*step*2+step*2);
			dc.MoveTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2);
			dc.LineTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2+step*2);
			///V
			dc.MoveTo(x0+num+pt.x*step*2,y0+pt.y*step*2+num4);
			dc.LineTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2+num4);
			dc.MoveTo(x0+num+pt.x*step*2,y0+pt.y*step*2+step*2+num4);
			dc.LineTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2+step*2+num4);
			dc.MoveTo(x0+num+pt.x*step*2,y0+pt.y*step*2+num4);
			dc.LineTo(x0+num+pt.x*step*2,y0+pt.y*step*2+step*2+num4);
			dc.MoveTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2+num4);
			dc.LineTo(x0+num+pt.x*step*2+step*2,y0+pt.y*step*2+step*2+num4);
		}
		edgeCount++;
	//	rect.bottom++;
	//		rect.right++;
	//		InvalidateRect(&rect);//
}
void ini()
{
	edgeCount=0;
	m_cursor_prev_old.SetPoint(-999,-999);
	m_cursor_prev_flag.SetPoint(-999,-999);
	Edge_Recv=Edge_Flag=true;
}
	void RefreshCaption()
	{
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		int x,y;
		//Y的表格范围
		if(pt.x >= x0 && pt.x <= x0+step*64 && pt.y>=y0 && pt.y <= y0 + step*64 )
		{
			x=((pt.x-x0)/(step*4));
			y=((pt.y-y0)/(step*4));
			CString captain;
			if(m_curContext == 2)
				captain.Format(L"Different  Frame:%d  (%d,%d)  Z-Index=%d",curFrame,x,y,pos[y][x]);
			else
				captain.Format(L"File:%d  Frame:%d  (%d,%d)  Z-Index=%d",m_curContext+1,curFrame,x,y,pos[y][x]);
			this->SetWindowTextW(captain);
		}
		else
		{
			CString captain;
			if(m_curContext == 2)
				captain.Format(L"Different  Frame:%d  (%d,%d)  Z-Index=%d",curFrame,-1,-1,-1);
			else
				captain.Format(L"File:%d  Frame:%d  (%d,%d)  Z-Index=%d",m_curContext+1,curFrame,-1,-1,-1);
			this->SetWindowTextW(captain);
		}
	}
// 对话框数据
	enum { IDD = IDD_DIFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
