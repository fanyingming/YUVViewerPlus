
// YUVviewerPlusDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "YUVviewerPlus.h"
#include "YUVviewerPlusDlg.h"
#include "Help.h"
#include "OptionDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_PLAY 0
static int g_timers[1];

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CYUVviewerPlusDlg �Ի���




CYUVviewerPlusDlg::CYUVviewerPlusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CYUVviewerPlusDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	open2Flag=false;
	open1Flag=false;
	showDiffFlag=false;
	listCount=0;
	m_link[0]=m_link[1]=m_link[2]=NULL;
}

void CYUVviewerPlusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RES, m_listRes);
}

BEGIN_MESSAGE_MAP(CYUVviewerPlusDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(IDC_BUTTON_OPEN1,IDC_BUTTON_OPEN4, &CYUVviewerPlusDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CYUVviewerPlusDlg::OnBnClickedButtonPlay)
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_LIST_RES, &CYUVviewerPlusDlg::OnLbnSelchangeListRes)
	ON_BN_CLICKED(IDC_BUTTON_BACKTOZERO, &CYUVviewerPlusDlg::OnBnClickedButtonBacktozero)
	ON_BN_CLICKED(IDC_BUTTON_BACKTOZERO2, &CYUVviewerPlusDlg::OnBnClickedButtonResetall)
	ON_BN_CLICKED(IDC_BUTTON_NEXTFRAME, &CYUVviewerPlusDlg::OnBnClickedButtonNextframe)
	ON_BN_CLICKED(IDC_BUTTON_SHOWDIFF, &CYUVviewerPlusDlg::OnBnClickedButtonShowdiff)
	ON_BN_CLICKED(IDC_BUTTON_PREVFRAME, &CYUVviewerPlusDlg::OnBnClickedButtonPrevframe)
	ON_BN_CLICKED(IDC_BUTTON_OPEN1, &CYUVviewerPlusDlg::OnBnClickedButtonOpen1)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, &CYUVviewerPlusDlg::OnEnChangeEditWidth)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CYUVviewerPlusDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CYUVviewerPlusDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_ADD2, &CYUVviewerPlusDlg::OnBnClickedButtonAdd2)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CYUVviewerPlusDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_HELP, &CYUVviewerPlusDlg::OnBnClickedButtonHelp)
ON_BN_CLICKED(IDC_BUTTON_OPEN3, &CYUVviewerPlusDlg::OnBnClickedButtonOpen3)
ON_WM_DROPFILES()
ON_WM_CREATE()
ON_MESSAGE_VOID(WM_CLOSE,myClose) //����ӵ�map
END_MESSAGE_MAP()





// CYUVviewerPlusDlg ��Ϣ�������
void CYUVviewerPlusDlg::myClose()
{
        //AfxMessageBox("exit");
        //ExitProcess(-1);
        

      //  if(::MessageBox(NULL, _T("Save the current interface before exit��"),_T("Program Exit"), MB_YESNO) == IDYES)
        {
                //������һЩ�������ݵĶ���
                this->OnClose();
        }
}

BOOL CYUVviewerPlusDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	UpdateList();
	SetDefaultItem();
	//����Ĭ��ÿ�벥��֡�������Ƕ�ʱ���趨ʱ��
	GetDlgItem(IDC_EDIT_FRAME)->SetWindowText(_T("30"));
	MoveWindow(50,20,515,157,true);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}
void CYUVviewerPlusDlg::SetDefaultItem()
{
	//ѡ�����ȼ�������ѡ��
	CStdioFile File;
	int index;
	CString wstr,hstr,priostr,strtemp;
	CString widthStr,heightStr;
	int nIndex=0,prio=-999,count=0;
	if(File.Open(_T(".\\Config.dat"),CFile::modeReadWrite))
	{
		while(File.ReadString(strtemp))
		{
			File.ReadString(priostr);
			int tempPrio=_ttoi(priostr);
			index=strtemp.Find('x');
			wstr=strtemp.Left(index);
			hstr=strtemp.Right(strtemp.GetLength()-index-1);
			if(hstr.GetAt(0)=='x')
				hstr.Delete(0,1);
			if(tempPrio > prio)
			{
				prio=tempPrio;
				nIndex=count;
				widthStr=wstr;
				heightStr=hstr;
			}
			count++;
		}
		File.Close();
		m_listRes.SetCurSel(nIndex);
		GetDlgItem(IDC_EDIT_WIDTH)->SetWindowTextW((LPWSTR)(LPCTSTR)widthStr);
		GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowTextW((LPWSTR)(LPCTSTR)heightStr);
	}
	else
	{
		MessageBox(_T("Config.dat�����ļ����ڵ�ǰĿ¼!"));
	}
}
void CYUVviewerPlusDlg::UpdateList()
{
	CStdioFile File;
	int width,height,index;
	CString wstr,hstr,priostr;
	if(File.Open(_T(".\\Config.dat"),CFile::modeReadWrite))
	{
		CString str=_T(""),strtemp=_T("");
		while(File.ReadString(strtemp))
		{
			index=strtemp.Find('x');
			wstr=strtemp.Left(index+1);
			hstr=strtemp.Right(strtemp.GetLength()-index-1);
			if(hstr.GetAt(0)=='x')
				hstr.Delete(0,1);
			width=_ttoi(wstr);
			height=_ttoi(hstr);
			m_listRes.AddString(strtemp);
			m_listRes.SetItemData(listCount++,width*10000+height);
			File.ReadString(strtemp);

		}
		File.Close();
	}
	else
		MessageBox(_T("Config.dat�����ļ����ڵ�ǰĿ¼!"));

}
void CYUVviewerPlusDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CYUVviewerPlusDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CYUVviewerPlusDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CYUVviewerPlusDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN )
	{
		if(S_OK==OnKeyDown(pMsg->wParam,pMsg->lParam))
			return 1;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CYUVviewerPlusDlg::OnBnClickedButtonOpen(UINT nID)
{
	int width,height;
	TCHAR sz[50];
	GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(sz,50);
	width=_ttoi(sz);
	GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(sz,50);
	height=_ttoi(sz);
	if(chooseFlag && width != 0 && height != 0)
	{
		CString   strFilter = _T("YUV File (*.yuv)|*.yuv|");
		CFileDialog dlgOpen(true, _T(".yuv"), NULL, OFN_HIDEREADONLY, strFilter, NULL);
		if(IDOK==dlgOpen.DoModal())
		{
			if(nID-IDC_BUTTON_OPEN1==0)//IDC_BUTTON_OPEN1=1003
				open1Flag=true;
			if(nID-IDC_BUTTON_OPEN1==2)
				open2Flag=true;
			KillTimer(TIMER_PLAY);
			g_timers[TIMER_PLAY]=0;
			OpenYUV(dlgOpen.GetPathName(),nID-IDC_BUTTON_OPEN1);
		//	ResolutionSwitch(0);
		}
	}
	else
		MessageBox(_T("����ѡ��ֱ���!"));

}
HRESULT CYUVviewerPlusDlg::OpenYUV(CString pathName,int ContextID)
{
	int width,height;
	TCHAR sz[50];
	GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(sz,50);
	width=_ttoi(sz);
	GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(sz,50);
	height=_ttoi(sz);
	if(ContextID==0)
		m_yuvm1.CreateNewSeq1(width,height,pathName,0);
	if(ContextID==2)
		m_yuvm2.CreateNewSeq1(width,height,pathName,1);
	return S_OK;
}
#define TIMER_PLAY 0
void CYUVviewerPlusDlg::OnBnClickedButtonPlay()
{
		if(g_timers[TIMER_PLAY])
		{
			KillTimer(TIMER_PLAY);
			g_timers[TIMER_PLAY]=0;
		}
		else
		{
			TCHAR sz[50];
			GetDlgItem(IDC_EDIT_FRAME)->GetWindowText(sz,50);
			int frame =_ttoi(sz);
			SetTimer(TIMER_PLAY,1000/frame,NULL);
			g_timers[TIMER_PLAY]=1;
		}
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CYUVviewerPlusDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nIDEvent==TIMER_PLAY)
	{	
		//���ͬʱ�����ļ�1���ļ�2
		if(open1Flag && open2Flag)
		{
			if(m_yuvm1.Next() || m_yuvm2.Next())
			{
				KillTimer(TIMER_PLAY);
				g_timers[TIMER_PLAY]=0;
			}
			else
			{//ʹ��3������ͬʱ����
				if(showDiffFlag)
				{
					m_yuvm1.m_context[0]->GetYUVBuffer(m_yuvm3.m_YUVbuffer1);
					m_yuvm2.m_context[1]->GetYUVBuffer(m_yuvm3.m_YUVbuffer2);
					if(m_yuvm3.ShowDiff(m_yuvm1.GetCurrentFrame()))
					{
						CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_STOP);
						int state = pBtn->GetCheck();
						if(state == 1)
						{
							KillTimer(TIMER_PLAY);
							g_timers[TIMER_PLAY]=0;
							MessageBox(_T("��ǰ֡����ͬ!"));
						}
					}
				}
			}
		}//����򿪵Ľ����ļ�1
		else if(open1Flag)
		{
			if(m_yuvm1.Next())
			{
				KillTimer(TIMER_PLAY);
				g_timers[TIMER_PLAY]=0;
			}
		}
		else if(open2Flag) //����򿪵Ľ����ļ�2
		{
			if(m_yuvm2.Next())
			{
				KillTimer(TIMER_PLAY);
				g_timers[TIMER_PLAY]=0;
			}
		}
		else
		{
			KillTimer(TIMER_PLAY);
			g_timers[TIMER_PLAY]=0;
		//	MessageBox(_T("��ѡ��Ҫ���ŵ��ļ�!"));//�����ע�ͣ��ڹر�ʱ������bug
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CYUVviewerPlusDlg::OnLbnSelchangeListRes()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int tmp=m_listRes.GetCurSel();
	if(tmp==-1)
		return;
	int widthheight=m_listRes.GetItemData(tmp);
	CString sz;
	sz.Format(_T("%d"),widthheight/10000);
	GetDlgItem(IDC_EDIT_WIDTH)->SetWindowText(sz);
	sz.Format(_T("%d"),widthheight%10000);
	GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowText(sz);
	chooseFlag=true;
}

LRESULT CYUVviewerPlusDlg::OnKeyDown(WPARAM wp,LPARAM lp)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	static int fullscreen=0;
	switch(wp)
	{
		case VK_RIGHT:
			NextFrame();
			return S_OK;
		case VK_LEFT:
			PrevFrame();
			return S_OK;
		case VK_SPACE:
			OnBnClickedButtonPlay();
			return S_OK;
		case VK_ESCAPE:
			OnBnClickedButtonResetall();
			return S_OK;
		case 'r':
		case 'R':
			OnBnClickedButtonBacktozero();
			return S_OK;
		case 'd':
		case 'D':
			OnBnClickedButtonShowdiff();
			return S_OK;
/*		case VK_DOWN:
			m_yuvm.ContrastDown();
			return S_OK;
		case VK_RETURN:
			m_yuvm.FullScreen(fullscreen=!fullscreen);
			return S_OK;
		case 'q':
		case 'Q':
			m_yuvm.SwitchContext(0);
			return S_OK;
		case 'w':
		case 'W':
			m_yuvm.SwitchContext(1);
			return S_OK;
		case 'e':
		case 'E':
			m_yuvm.SwitchContext(2);
			return S_OK;
		case 'r':
		case 'R':
			m_yuvm.SwitchContext(3);
			return S_OK;
		case 'a':
		case 'A':
			m_yuvm.SetDisplayMode(DM_SINGLE);
			return S_OK;
		case 's':
		case 'S':
			m_yuvm.SetDisplayMode(DM_HOR);
			return S_OK;
		case 'd':
		case 'D':
			m_yuvm.SetDisplayMode(DM_VER);
			return S_OK;
		case 'f':
		case 'F':
			m_yuvm.SetDisplayMode(DM_CUBE);
			return S_OK;
		
			*/
	};
	
	return S_FALSE;
}

void CYUVviewerPlusDlg::OnBnClickedButtonBacktozero()
{
	KillTimer(TIMER_PLAY);
	g_timers[TIMER_PLAY]=0;
	if(open1Flag)
		m_yuvm1.BackToZero();
	if(open2Flag)
		m_yuvm2.BackToZero();
	if(showDiffFlag)
	{
		m_yuvm1.m_context[0]->GetYUVBuffer(m_yuvm3.m_YUVbuffer1);
		m_yuvm2.m_context[1]->GetYUVBuffer(m_yuvm3.m_YUVbuffer2);
		m_yuvm3.ShowDiff(m_yuvm1.GetCurrentFrame());
	}
}

void CYUVviewerPlusDlg::OnBnClickedButtonResetall()
{
	KillTimer(TIMER_PLAY);
	g_timers[TIMER_PLAY]=0;
	open2Flag=false;
	open1Flag=false;
	showDiffFlag=false;
	m_yuvm1.Reset();
	m_yuvm2.Reset();
	m_yuvm3.Reset();
	GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(1);
	GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(1);
	GetDlgItem(IDC_LIST_RES)->EnableWindow(1);
	listCount=0;
	m_link[0]=m_link[1]=m_link[2]=NULL;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CYUVviewerPlusDlg::NextFrame()
{
	if(open1Flag)
		m_yuvm1.Next();
	if(open2Flag)
		m_yuvm2.Next();
	if(showDiffFlag  )
	{
		if(open1Flag && open2Flag)
		{
			m_yuvm1.m_context[0]->GetYUVBuffer(m_yuvm3.m_YUVbuffer1);
			m_yuvm2.m_context[1]->GetYUVBuffer(m_yuvm3.m_YUVbuffer2);
			if(m_yuvm3.ShowDiff(m_yuvm1.GetCurrentFrame()))
			{
				CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_STOP);
				int state = pBtn->GetCheck();
				if(state == 1)
				{
					KillTimer(TIMER_PLAY);
					g_timers[TIMER_PLAY]=0;
					MessageBox(_T("��ǰ֡����ͬ!"));
				}
			}
		}
		else
			MessageBox(_T("��2��ͼ����ܶԱ�!"));
	}
}

void CYUVviewerPlusDlg::OnBnClickedButtonNextframe()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	NextFrame();
}


void CYUVviewerPlusDlg::OnBnClickedButtonShowdiff()
{
	if(open1Flag && open2Flag)//����2����Ƶ�ļ�
	{
		m_yuvm3.Clean();
		int width,height;
		TCHAR sz[50];
		GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(sz,50);
		width=_ttoi(sz);
		GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(sz,50);
		height=_ttoi(sz);
		m_yuvm3.Init(width,height,m_yuvm1.GetCurrentFrame());
		m_yuvm1.m_context[0]->GetYUVBuffer(m_yuvm3.m_YUVbuffer1);
		//��m_yuvm1.m_context[0]��YUV����m_yuvm3.m_YUVbuffer1
		m_yuvm2.m_context[1]->GetYUVBuffer(m_yuvm3.m_YUVbuffer2);
		if(m_yuvm3.ShowDiff(m_yuvm1.GetCurrentFrame()))
		{
			CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_STOP);
			int state = pBtn->GetCheck();
			if(state == 1)
			{
				KillTimer(TIMER_PLAY);
				g_timers[TIMER_PLAY]=0;
				MessageBox(_T("��ǰ֡����ͬ!"));
			}
		}
		m_link[0]=m_yuvm1.GetLink();
		m_link[1]=m_yuvm2.GetLink();
		m_link[2]=m_yuvm3.GetLink();
		m_yuvm1.m_shDlg->SetLink(m_link);
		m_yuvm2.m_shDlg->SetLink(m_link);
		m_yuvm3.m_shDlg->SetLink(m_link);
		showDiffFlag=true;
		m_yuvm1.m_context[0]->GetYUVBuffer(m_yuvm3.m_YUVbuffer1);
		m_yuvm2.m_context[1]->GetYUVBuffer(m_yuvm3.m_YUVbuffer2);
		m_yuvm3.ShowDiff(m_yuvm1.GetCurrentFrame());
	}
	else
		MessageBox(_T("����ѡ��Ҫ���Ե���Ƶ�ļ�!"));
}


void CYUVviewerPlusDlg::PrevFrame()
{
	if(open1Flag)
		m_yuvm1.Prev();
	if(open2Flag)
		m_yuvm2.Prev();
	if(showDiffFlag)
	{
		m_yuvm1.m_context[0]->GetYUVBuffer(m_yuvm3.m_YUVbuffer1);
		m_yuvm2.m_context[1]->GetYUVBuffer(m_yuvm3.m_YUVbuffer2);
		if(m_yuvm3.ShowDiff(m_yuvm1.GetCurrentFrame()))
		{
			CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_STOP);
			int state = pBtn->GetCheck();
			if(state == 1)
			{
				KillTimer(TIMER_PLAY);
				g_timers[TIMER_PLAY]=0;
				MessageBox(_T("��ǰ֡����ͬ!"));
			}
		}
	}
}

void CYUVviewerPlusDlg::OnBnClickedButtonPrevframe()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PrevFrame();
}

void CYUVviewerPlusDlg::OnBnClickedButtonOpen1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CYUVviewerPlusDlg::OnEnChangeEditWidth()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void CYUVviewerPlusDlg::OnBnClickedButtonAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CStdioFile File;
	bool find=false;
	int width,height;
	CString strtemp,wstr,hstr,copy,temp;
	if(File.Open(_T(".\\Config.dat"),CFile::modeReadWrite))
	{
		TCHAR sz[50];
		GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(sz,50);
		width=_ttoi(sz);
		GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(sz,50);
		height=_ttoi(sz);
		if(width>0 && height> 0)
		{
			wstr.Format(_T("%d"),width);
			hstr.Format(_T("%d"),height);
			temp = wstr + "x" + hstr; 
			//Ѱ����û���ظ�
			while(File.ReadString(strtemp))
			{
				if(strtemp.Find(temp)<0)//δ�ҵ�
				{
					copy += _T("\r\n");
					copy += strtemp ;
				}
				else
				{
					find=true;
					break;
				}//�ҵ�����������������
			}
			if(!find)
			{
				m_listRes.AddString(wstr + "x" + hstr);
				m_listRes.SetItemData(listCount++,width*10000+height);
				m_listRes.SetCurSel(listCount-1);
				File.Close();
				File.Open(_T(".\\Config.dat"),CFile::modeCreate|CFile::modeReadWrite);
				CString prio;
				prio.Format(_T("%d"),100);
				temp="\r\n";
				strtemp = temp + wstr ;
				temp="x";
				strtemp += temp + hstr;
				temp="\r\n";
				strtemp += temp + prio;
				copy += strtemp;
				copy.Delete(0,2);
				File.WriteString(copy);
				File.Close();
				ClearList();UpdateList();SetDefaultItem();
			}
		}
	}
	else
		MessageBox(_T("Config.dat�����ļ����ڵ�ǰĿ¼!"));
}

void CYUVviewerPlusDlg::OnBnClickedButtonDelete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int tmp=m_listRes.GetCurSel();
	if(tmp==-1)
		return;
	CStdioFile File;
	if(File.Open(_T(".\\Config.dat"),CFile::modeReadWrite))
	{
		//�õ�width height
		CString strtemp,wstr,hstr,temp,copy;
		int widthheight=m_listRes.GetItemData(tmp);
		wstr.Format(_T("%d"),widthheight/10000);
		hstr.Format(_T("%d"),widthheight%10000);
		temp = wstr + _T("x")+ hstr;
		while(File.ReadString(strtemp))
		{
			if(strtemp.Find(temp)<0)
			{copy += _T("\r\n");copy += strtemp ;}
			else
			{File.ReadString(strtemp);}//�ҵ����������Ϊ��ɾ�����ȼ�
		}
		File.Close();
		File.Open(_T(".\\Config.dat"),CFile::modeCreate|CFile::modeReadWrite);
		copy.Delete(0,2);
		File.WriteString(copy);
		File.Close();
		m_listRes.DeleteString(tmp);
		GetDlgItem(IDC_EDIT_WIDTH)->SetWindowTextW(_T(""));
		GetDlgItem(IDC_EDIT_HEIGHT)->SetWindowTextW(_T(""));
		SetDefaultItem();
	}
	else
		MessageBox(_T("Config.dat�����ļ����ڵ�ǰĿ¼!"));
}

void CYUVviewerPlusDlg::OnBnClickedButtonAdd2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int tmp=m_listRes.GetCurSel();
	if(tmp==-1)
		return;
	CStdioFile File;
	CString strtemp,wstr,hstr,temp,copy,update;
	int width,height;
	if(File.Open(_T(".\\Config.dat"),CFile::modeReadWrite))
	{
		//�õ�Listѡ�е�width height
		int widthheight=m_listRes.GetItemData(tmp);
		wstr.Format(_T("%d"),widthheight/10000);
		hstr.Format(_T("%d"),widthheight%10000);
		temp = wstr + _T("x")+ hstr;
		//�õ�Ҫ�޸ĳɵ�width height
		TCHAR sz[50];
		GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(sz,50);
		width=_ttoi(sz);
		GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(sz,50);
		height=_ttoi(sz);
		if(width>0 && height> 0)//Ҫ�޸ĵķֱ��ʱ���Ϸ�
		{
			wstr.Format(_T("%d"),width);
			hstr.Format(_T("%d"),height);
			update = wstr + _T("x")+ hstr;
			m_listRes.DeleteString(tmp);
			m_listRes.InsertString(tmp,update);
			m_listRes.SetItemData(tmp,width*10000+height);
			m_listRes.SetCurSel(tmp);
			//�޸��ļ��е���ֵ
			while(File.ReadString(strtemp))
			{
				if(strtemp.Find(temp)<0)//��ǰ�Ҳ���Listѡ�е�Ҫ�޸ķֱ���
				{
					copy += _T("\r\n");
					copy += strtemp ;
				}
				else
				{
					copy += _T("\r\n");
					copy += update ;
				}
			}
			File.Close();
			File.Open(_T(".\\Config.dat"),CFile::modeCreate|CFile::modeReadWrite);
			copy.Delete(0,2);
			File.WriteString(copy);
			File.Close();
		}
	}
	else
	{
		MessageBox(_T("Config.dat�����ļ����ڵ�ǰĿ¼!"));
	}
}
void CYUVviewerPlusDlg::OnBnClickedButtonClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OptionDlg dlg=new OptionDlg();
	if(dlg.DoModal()==IDOK)
	{ClearList();UpdateList();SetDefaultItem();}

	else
		SetDefaultItem();
}

void CYUVviewerPlusDlg::OnBnClickedButtonHelp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Help help=new Help();
	help.DoModal();
//	help.Create(IDD_HELP,NULL);
//	help.ShowWindow(SW_SHOW);
}

void CYUVviewerPlusDlg::OnBnClickedButtonOpen3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CYUVviewerPlusDlg::OnDropFiles(HDROP hDropInfo)
{
	
	const int fileCount = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
	TCHAR fileName[MAX_PATH] = { 0 };
	CString fn;
	if(fileCount > 2)
	{
		MessageBox(_T("��ǰ��2֡�Ա�ģʽ��ֻ֧�ִ�2���ļ�!"));
		return;
	}
	else if(fileCount == 1)//ѡ��2֡�Ա�ģʽ����һ���ļ���
	{
		DragQueryFile(hDropInfo, 0, fileName, MAX_PATH);//����ļ�·��
		fn.Format(_T("%s"),fileName);
		int index=fn.Find('.');
		CString name;
		name=fn.Right(fn.GetLength()-index-1);
		if(name.Find(_T("yuv"))<0)
		{
			MessageBox(_T("�ļ���ʽ������.yuv��"));
			return;
		}
		if(!open1Flag)
		{
			open1Flag=true;
			m_yuvm1.Clean();
			OpenYUV(fileName,0);
		}
		else if(!open2Flag)
		{
			open2Flag=true;
			m_yuvm2.Clean();
			OpenYUV(fileName,2);
		}
		else
			MessageBox(_T("ֻ֧��2���ļ��Ա�!"));
	}
	else
	{
		ASSERT(fileCount >= 1);
		TCHAR fileName[MAX_PATH] = { 0 };
		for (int i = 0; i < fileCount; i++)
		{
			DragQueryFile(hDropInfo, i, fileName, MAX_PATH);
			int width,height;
			TCHAR sz[50];
			GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(sz,50);
			width=_ttoi(sz);
			GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(sz,50);
			height=_ttoi(sz);
			if(width > 0 && height > 0)
			{		
				if(i==0)
				{
					open1Flag=true;
					m_yuvm1.Clean();
					OpenYUV(fileName,0);
				}
				if(i==1)
				{
					open2Flag=true;
					m_yuvm2.Clean();
					OpenYUV(fileName,2);
				}
				KillTimer(TIMER_PLAY);
				g_timers[TIMER_PLAY]=0;
			//	ResolutionSwitch(0);
			}
			else
				MessageBox(_T("�ֱ��ʴ���!"));
		}
	}
	CDialog::OnDropFiles(hDropInfo);
}

int CYUVviewerPlusDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO:  �ڴ������ר�õĴ�������
	DragAcceptFiles(TRUE);
	return 0;
}
