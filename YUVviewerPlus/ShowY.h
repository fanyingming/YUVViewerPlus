#pragma once


// ShowY 对话框
#include "alignmemory.h"
#define PIC_SIZE ((m_width*m_height*3)>>1)
class ShowY : public CDialog
{
	DECLARE_DYNAMIC(ShowY)

public:
	ShowY(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ShowY();

	void SetBuffer(unsigned char * yb1,unsigned char * yb2,unsigned char * ub1,unsigned char * ub2,unsigned char * vb1,unsigned char * vb2)
	{
		unsigned char* y1 = yb1;
		unsigned char* y2 = yb2;
		unsigned char* u1 = ub1;
		unsigned char* u2 = ub2;
		unsigned char* v1 = vb1;
		unsigned char* v2 = vb2;
		int count=0,height=64,width=64,countuv=0;
		for ( int i = 0; i < height; i++ )
		{
			for ( int j = 0; j < width; j++ )
			{
				if(j >= pox && j <= pox+3 && i >= poy && i <= poy+3)
				{
					yBuffer1[count]=y1[j];
					yBuffer2[count]=y2[j];
					if(j%2==0 && i%2==0)
					{
						uBuffer1[countuv]=u1[j/2];
						uBuffer2[countuv]=u2[j/2];
						vBuffer1[countuv]=v1[j/2];
						vBuffer2[countuv]=v2[j/2];
						countuv++;
					}
					count++;
				}
			}
			y1 += width;
			y2 += width;
			if(i%2==1)
			{
				u1 += width / 2;
				v1 += width / 2;
				u2 += width / 2;
				v2 += width / 2;
			}
		}
	}
	void SetWH(int width,int height)
	{
		m_height=height;
		m_width=width;
	}
    void SetData(int Zindex,int x,int y)
	{
		index=Zindex;
		pox=x;
		poy=y;
	}
// 对话框数据
	enum { IDD = IDD_SHOWY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
private:
	char *yBuffer1,*yBuffer2,*uBuffer1,*uBuffer2,*vBuffer1,*vBuffer2;
	int m_width,m_height;
	int index,pox,poy;
	int counter;
	int m_curContext;
	int curFrame;
	DECLARE_MESSAGE_MAP()
public:
	void SetCurFrame(int n)
	{
		curFrame=n;
	}
	void SetCurContext(int n)
	{
		m_curContext=n;
	}
	void RefreshCaption()
	{
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		CString captain;
		if(m_curContext == 2)
			captain.Format(L"Different  Frame:%d  Y/U/V Value",curFrame);
		else
			captain.Format(L"File:%d  Frame:%d  Y/U/V Value",m_curContext+1,curFrame);
		this->SetWindowTextW(captain);
	
	}
	afx_msg void OnPaint();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
