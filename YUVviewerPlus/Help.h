#pragma once


// Help �Ի���

class Help : public CDialog
{
	DECLARE_DYNAMIC(Help)

public:
	Help(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Help();

// �Ի�������
	enum { IDD = IDD_HELP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};
