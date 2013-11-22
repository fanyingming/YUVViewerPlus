#pragma once
#include "stdafx.h"
#include "resource.h"
#include "alignmemory.h"
#include "DiffDialog.h"

#define PIC_SIZE ((m_width*m_height*3)>>1)
typedef struct
{
	int curframe;
	int contrast;
	int contextid;
	CString filename;
}DisplayInfo;

// ShowDlg 对话框

class ShowDlg : public CDialog
{
	DECLARE_DYNAMIC(ShowDlg)

public:
	ShowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ShowDlg();

// 对话框数据
	enum { IDD = IDD_SHOWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	int m_width,m_height;
	CPoint m_cursor_prev;
	char *buffer;
	BITMAPINFO m_bmi;
	HANDLE m_mutex;
	CWnd *p_mainWnd;
	DisplayInfo m_info;
	char *m_YUVBuffer1,*m_YUVBuffer2;
	bool IsSame;
	DiffDialog *diff;
	int clickCount;
	ShowDlg *m_link[3];
	CPoint m_cursor_prev_old;
	char *flagBuffer;
	bool firstFlag;
	int m_curContext;

public:
	virtual BOOL OnInitDialog();

public:

	void SetCurContext(int n)
	{
		m_curContext=n;
	}
	bool GetIsSame()
	{
		return IsSame;
	}
	void SetLink(ShowDlg *p[])
	{
		m_link[0]=p[0];
		m_link[1]=p[1];
		m_link[2]=p[2];
	}
	
	void SetBitmap(char *bits);
	void SetIsSame(bool b)
	{
		IsSame=b;
	}
	void SetWH(int width,int height)
	{
		bool dirty=0;
		if(m_width!=width||m_height!=height)
			dirty=1;
		m_width=width;
		m_height=height;
		BITMAPINFO bmi;
		bmi.bmiHeader.biBitCount=32;
		bmi.bmiHeader.biHeight=m_height;
		bmi.bmiHeader.biWidth=m_width;
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biCompression = BI_RGB;
		m_bmi=bmi;
		if(dirty)
		{
			if(buffer)
				align_free(buffer);
			buffer=(char*)align_malloc(m_width*m_height*4);
			CRect wndRect,cliRect;
			this->GetWindowRect(&wndRect);
			this->GetClientRect(&cliRect);
			this->SetWindowPos(NULL,0,0,m_width+wndRect.Width()-cliRect.Width(),m_height+wndRect.Height()-cliRect.Height(),SWP_NOMOVE);
		}
	}
	void ShowSame()
	{
		CClientDC dc(this);
		CRect rect;
		GetClientRect(rect);
	    dc.FillSolidRect(rect,RGB(255,255,255));
		dc.TextOutW(20,20,_T("图像相同"));
	}
	void ReturnToImageSize()
	{
		SetWindowPos(NULL,0,0,m_width,m_height,SWP_NOMOVE);
	}
	void SetMainWnd(CWnd *p)
	{
		p_mainWnd=p;
	}
	void SetDisplayInfo(DisplayInfo info)
	{
		WaitForSingleObject(m_mutex,INFINITE);
		if(info.curframe!=-1)
			m_info.curframe=info.curframe;
		/*
		if(info.contrast!=-1)
			m_info.contrast=info.contrast;
		*/
		if(info.filename.GetLength())
			m_info.filename=info.filename;
		if(info.contextid!=-1)
			m_info.contextid=info.contextid;
		
		ReleaseMutex(m_mutex);
		RefreshCaption();
	}
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	LRESULT OnKeyDown(WPARAM wp,LPARAM lp)
	{
		::PostMessage(p_mainWnd->GetSafeHwnd(),WM_KEYDOWN,wp,lp);
		return S_OK;
	}
	void RefreshCaption();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

    void GetYUVBuffer(char* buffer1,char* buffer2)
	{
		memcpy(m_YUVBuffer1,buffer1,PIC_SIZE);
		memcpy(m_YUVBuffer2,buffer2,PIC_SIZE);
	}
	void YUVDiff(unsigned char* yuv1,unsigned char* yuv2,bool* result,CPoint pos)
	{
		int x = pos.x>>6;
		int y = pos.y>>6;
		x=x<<6;
		y=y<<6;
		bool *block;
		block=(bool *)align_malloc(64*64);//把选中方块的YUV是否相同截取出来
		unsigned char* y1 = yuv1;
		unsigned char* u1 = y1 + m_width * m_height;
		unsigned char* v1 = u1 + m_width * m_height / 4;
		unsigned char* y2 = yuv2;
		unsigned char* u2 = y2 + m_width * m_height;
		unsigned char* v2 = u2 + m_width * m_height / 4;
		int count=0;
		for ( int i = 0; i < m_height; i++ )
		{
			for ( int j = 0; j < m_width; j++ )
			{
				int cy1 = y1[j];
				int cu1 = u1[j / 2];
				int cv1 = v1[j / 2];
				int cy2 = y2[j];
				int cu2 = u2[j / 2];
				int cv2 = v2[j / 2];
				if(j>=x && j<=x+63 && i>=y && i<=y+63)
				{
					if(cy1 == cy2 && cu1 == cu2 && cv1 ==cv2)
						block[count]=1;
					else
						block[count]=0;
					count++;
					if(j == m_width-1)//超出右边界
					{
						for(int k = m_width-x;k<=63;k++)
						{
							block[count]=1;
							count++;
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
		//超出下边界
		if(count<64*64)
		{
			for(;count<64*64;count++)
			{
				block[count]=1;
			}
		}
		memcpy(result,block,64*64);
		align_free(block);
	}

private:
	void MosePoint_old(CPoint pt,bool refresh=1,bool link=1)
	{
		if(pt!=m_cursor_prev_old)
		{
			if(clickCount == 0)//第一次点击
			{	DrawBlockEdge(pt);//取反
				m_cursor_prev_old=pt;
				DrawBlockEdge_Rem(m_cursor_prev_old);//记住像素
				DrawBlockEdge_Flag(m_cursor_prev_old);//标记像素
			}
			else
			{
				DrawBlockEdge_Recv(m_cursor_prev_old);//恢复像素
				m_cursor_prev_old=pt;
				DrawBlockEdge(pt);
				DrawBlockEdge_Rem(m_cursor_prev_old);
				DrawBlockEdge_Flag(m_cursor_prev_old);
			}
		}
		clickCount++;
		if(link)
			for(int i=0;i<3;i++)
				if(m_link[i] )
				{
					if(i==2)
					{
						if(!m_link[2]->GetIsSame())
							m_link[i]->MousePoint(pt,0,0);
					}
					else
						m_link[i]->MousePoint(pt,0,0);
				}
	}
	void MousePoint(CPoint pt,bool refresh=1,bool link=1)
	{
		if(pt!=m_cursor_prev)
		{
			DrawBlockEdge(m_cursor_prev);
			m_cursor_prev=pt;
			DrawBlockEdge(m_cursor_prev);
			if(refresh)
				RefreshCaption();
		}
		if(link)
			for(int i=0;i<3;i++)
				if(m_link[i] )
				{
					if(i==2)
					{
						//使得当2个图像相同时，在图像上的移动不会响应在不同画面上。
						if(!m_link[2]->GetIsSame())
							m_link[2]->MousePoint(pt,0,0);
					}
					else
						m_link[i]->MousePoint(pt,0,0);
				}
		
	}

	void DrawBlockEdge(CPoint pt)
	{
		WaitForSingleObject(m_mutex,INFINITE);
		CRect rect;
		rect.SetRect(pt.x<<6,pt.y<<6,(pt.x<<6)+63,(pt.y<<6)+63);
		{
			int offset;
			offset=(m_height-rect.top-1)*m_width;
			for(int i=0;i<64;i++)
			{
				int step=(i==0||i==63)?1:63;//step代表的是顶和底
				if(offset<m_width*m_height && offset>=0)
					for(int j=0;j<64;j+=step)
					{
					//	if(rect.left+j<m_width&&rect.left+j>=0)
						if(rect.left+j<m_width)//防止矩形超出边界，右边界
						{
							int r=(offset+rect.left+j)*4;
							buffer[r]=~buffer[r];
							r++;
							buffer[r]=~buffer[r];
							r++;
							buffer[r]=~buffer[r];
						}
					}
				offset-=m_width;//????
			}
		}
		rect.bottom++;
		rect.right++;
		ReleaseMutex(m_mutex);
		InvalidateRect(&rect);//
	}
	void DrawBlockEdge_Flag(CPoint pt)
	{
		WaitForSingleObject(m_mutex,INFINITE);
		CRect rect;
		rect.SetRect(pt.x<<6,pt.y<<6,(pt.x<<6)+63,(pt.y<<6)+63);
		{
			int offset;
			offset=(m_height-rect.top-1)*m_width;
			for(int i=0;i<64;i++)
			{
				int step=(i==0||i==63)?1:63;//step代表的是顶和底
				if(offset<m_width*m_height && offset>=0)
					for(int j=0;j<64;j+=step)
					{
					//	if(rect.left+j<m_width&&rect.left+j>=0
						if(rect.left+j<m_width)//防止矩形超出边界，右边界
						{
							int r=(offset+rect.left+j)*4;
							buffer[r]=0;
							r++;
							buffer[r]=255;
							r++;
							buffer[r]=0;
						}
					}
				offset-=m_width;//????
			}
		}
		rect.bottom++;
		rect.right++;
		ReleaseMutex(m_mutex);
		InvalidateRect(&rect);//
	}
	void DrawBlockEdge_Rem(CPoint pt)
	{
		WaitForSingleObject(m_mutex,INFINITE);
		CRect rect;
		rect.SetRect(pt.x<<6,pt.y<<6,(pt.x<<6)+63,(pt.y<<6)+63);
			int offset;
			offset=(m_height-rect.top-1)*m_width;
			int count=0;
			for(int i=0;i<64;i++)
			{
				int step=(i==0||i==63)?1:63;//step代表的是顶和底
				if(offset<m_width*m_height && offset>=0)
					for(int j=0;j<64;j+=step)
					{
					//	if(rect.left+j<m_width&&rect.left+j>=0)
						if(rect.left+j<m_width)//防止矩形超出边界，右边界
						{
							int r=(offset+rect.left+j)*4;
							flagBuffer[count++]=buffer[r];
							r++;
							flagBuffer[count++]=buffer[r];
							r++;
							flagBuffer[count++]=buffer[r];
						}
					}
				offset-=m_width;//????
			}
	}
	void DrawBlockEdge_Recv(CPoint pt)
	{
		WaitForSingleObject(m_mutex,INFINITE);
		CRect rect;
		rect.SetRect(pt.x<<6,pt.y<<6,(pt.x<<6)+63,(pt.y<<6)+63);
		int offset;
		offset=(m_height-rect.top-1)*m_width;
		int count=0;
		for(int i=0;i<64;i++)
		{
			int step=(i==0||i==63)?1:63;//step代表的是顶和底
			if(offset<m_width*m_height && offset>=0)
				for(int j=0;j<64;j+=step)
				{
					//	if(rect.left+j<m_width&&rect.left+j>=0)
					if(rect.left+j<m_width)//防止矩形超出边界，右边界
					{
						int r=(offset+rect.left+j)*4;
						buffer[r]=flagBuffer[count++];
						r++;
						buffer[r]=flagBuffer[count++];
						r++;
						buffer[r]=flagBuffer[count++];
					}
				}
				offset-=m_width;//????
		}
		rect.bottom++;
		rect.right++;
		ReleaseMutex(m_mutex);
		InvalidateRect(&rect);//
	}
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnClose();
};
