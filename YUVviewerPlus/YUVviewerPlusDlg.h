
// YUVviewerPlusDlg.h : ͷ�ļ�
//

#pragma once
#include "YUVmanager.h"
#include "afxwin.h"


// CYUVviewerPlusDlg �Ի���
class CYUVviewerPlusDlg : public CDialog
{
// ����
public:
	CYUVviewerPlusDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_YUVVIEWERPLUS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonOpen(UINT nID);
public:
	HRESULT OpenYUV(CString pathName,int ContextID);

	YUVManager m_yuvm1;//��Ƶ1
	YUVManager m_yuvm2;//��Ƶ2
	YUVManager m_yuvm3;//��ʾ��ͬ
private:	
	int m_width,m_height;
	bool chooseFlag;//�Ƿ�ѡ��ֱ���
	bool open2Flag;//�Ƿ�ѡ����Ƶ2
	bool open1Flag;//
	bool showDiffFlag;//ʱ��ѡ������ʾ��ͬ
	int listCount;
	bool singleVideo,doubleVideo;
	ShowDlg *m_link[3];
public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLbnSelchangeListRes();
	CListBox m_listRes;
	LRESULT OnKeyDown(WPARAM wp,LPARAM lp);
	afx_msg void OnBnClickedButtonBacktozero();
	afx_msg void OnBnClickedButtonResetall();
	afx_msg void OnBnClickedButtonNextframe();
	afx_msg void OnBnClickedButtonShowdiff();
	afx_msg void OnBnClickedButtonPrevframe();
	afx_msg void OnBnClickedButtonOpen1();
	afx_msg void OnEnChangeEditWidth();
	void NextFrame();
	void PrevFrame();
	afx_msg void OnBnClickedButtonAdd();
	void UpdateList();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonAdd2();
	afx_msg void OnBnClickedButtonClear();
	void myClose();
	afx_msg void OnBnClickedButtonHelp();
	afx_msg void OnBnClickedButtonOpen3();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void SetDefaultItem();
	void SetOpen1Flag(bool flag1)
	{
		open1Flag=flag1;
	}
	void SetOpen2Flag(bool flag2)
	{
		open2Flag=flag2;
	}
	void SetShowDiffFlag(bool flag3)
	{
		showDiffFlag=flag3;
	}
	void ClearList()
	{
		for(int i=m_listRes.GetCount()-1;i>=0;i--)
			m_listRes.DeleteString(i);
		listCount=0;
	}
	void ResolutionSwitch(int value)
	{
		GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(0);
		GetDlgItem(IDC_LIST_RES)->EnableWindow(0);
	}
	void closeWindow(int which)
	{
		if(which == 0)
		{
			SetOpen1Flag(false);
		}
		else if(which == 1)
		{
			SetOpen2Flag(false);

		}else
		{
			SetShowDiffFlag(false);
		}
	}
};
