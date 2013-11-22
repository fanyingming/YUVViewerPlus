
// YUVviewerPlusDlg.cpp : 实现文件
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

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CYUVviewerPlusDlg 对话框




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
ON_MESSAGE_VOID(WM_CLOSE,myClose) //新添加的map
END_MESSAGE_MAP()





// CYUVviewerPlusDlg 消息处理程序
void CYUVviewerPlusDlg::myClose()
{
        //AfxMessageBox("exit");
        //ExitProcess(-1);
        

      //  if(::MessageBox(NULL, _T("Save the current interface before exit？"),_T("Program Exit"), MB_YESNO) == IDYES)
        {
                //可以做一些保存数据的动作
                this->OnClose();
        }
}

BOOL CYUVviewerPlusDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// TODO: 在此添加额外的初始化代码
	UpdateList();
	SetDefaultItem();
	//设置默认每秒播放帧数，就是定时器设定时间
	GetDlgItem(IDC_EDIT_FRAME)->SetWindowText(_T("30"));
	MoveWindow(50,20,515,157,true);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CYUVviewerPlusDlg::SetDefaultItem()
{
	//选择优先级最大的项选中
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
		MessageBox(_T("Config.dat配置文件不在当前目录!"));
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
		MessageBox(_T("Config.dat配置文件不在当前目录!"));

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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CYUVviewerPlusDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CYUVviewerPlusDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CYUVviewerPlusDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
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
		MessageBox(_T("请先选择分辨率!"));

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
	// TODO: 在此添加控件通知处理程序代码
}


void CYUVviewerPlusDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nIDEvent==TIMER_PLAY)
	{	
		//如果同时打开了文件1和文件2
		if(open1Flag && open2Flag)
		{
			if(m_yuvm1.Next() || m_yuvm2.Next())
			{
				KillTimer(TIMER_PLAY);
				g_timers[TIMER_PLAY]=0;
			}
			else
			{//使得3个画面同时播放
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
							MessageBox(_T("当前帧不相同!"));
						}
					}
				}
			}
		}//如果打开的仅是文件1
		else if(open1Flag)
		{
			if(m_yuvm1.Next())
			{
				KillTimer(TIMER_PLAY);
				g_timers[TIMER_PLAY]=0;
			}
		}
		else if(open2Flag) //如果打开的仅是文件2
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
		//	MessageBox(_T("请选择要播放的文件!"));//如果不注释，在关闭时会引起bug
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CYUVviewerPlusDlg::OnLbnSelchangeListRes()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
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
	// TODO: 在此添加控件通知处理程序代码
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
					MessageBox(_T("当前帧不相同!"));
				}
			}
		}
		else
			MessageBox(_T("打开2幅图像才能对比!"));
	}
}

void CYUVviewerPlusDlg::OnBnClickedButtonNextframe()
{
	// TODO: 在此添加控件通知处理程序代码
	NextFrame();
}


void CYUVviewerPlusDlg::OnBnClickedButtonShowdiff()
{
	if(open1Flag && open2Flag)//打开了2个视频文件
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
		//将m_yuvm1.m_context[0]的YUV拷到m_yuvm3.m_YUVbuffer1
		m_yuvm2.m_context[1]->GetYUVBuffer(m_yuvm3.m_YUVbuffer2);
		if(m_yuvm3.ShowDiff(m_yuvm1.GetCurrentFrame()))
		{
			CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_STOP);
			int state = pBtn->GetCheck();
			if(state == 1)
			{
				KillTimer(TIMER_PLAY);
				g_timers[TIMER_PLAY]=0;
				MessageBox(_T("当前帧不相同!"));
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
		MessageBox(_T("请先选择要测试的视频文件!"));
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
				MessageBox(_T("当前帧不相同!"));
			}
		}
	}
}

void CYUVviewerPlusDlg::OnBnClickedButtonPrevframe()
{
	// TODO: 在此添加控件通知处理程序代码
	PrevFrame();
}

void CYUVviewerPlusDlg::OnBnClickedButtonOpen1()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CYUVviewerPlusDlg::OnEnChangeEditWidth()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CYUVviewerPlusDlg::OnBnClickedButtonAdd()
{
	// TODO: 在此添加控件通知处理程序代码
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
			//寻找有没有重复
			while(File.ReadString(strtemp))
			{
				if(strtemp.Find(temp)<0)//未找到
				{
					copy += _T("\r\n");
					copy += strtemp ;
				}
				else
				{
					find=true;
					break;
				}//找到后跳出，不做操作
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
		MessageBox(_T("Config.dat配置文件不在当前目录!"));
}

void CYUVviewerPlusDlg::OnBnClickedButtonDelete()
{
	// TODO: 在此添加控件通知处理程序代码
	int tmp=m_listRes.GetCurSel();
	if(tmp==-1)
		return;
	CStdioFile File;
	if(File.Open(_T(".\\Config.dat"),CFile::modeReadWrite))
	{
		//得到width height
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
			{File.ReadString(strtemp);}//找到后继续下移为了删掉优先级
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
		MessageBox(_T("Config.dat配置文件不在当前目录!"));
}

void CYUVviewerPlusDlg::OnBnClickedButtonAdd2()
{
	// TODO: 在此添加控件通知处理程序代码
	int tmp=m_listRes.GetCurSel();
	if(tmp==-1)
		return;
	CStdioFile File;
	CString strtemp,wstr,hstr,temp,copy,update;
	int width,height;
	if(File.Open(_T(".\\Config.dat"),CFile::modeReadWrite))
	{
		//得到List选中的width height
		int widthheight=m_listRes.GetItemData(tmp);
		wstr.Format(_T("%d"),widthheight/10000);
		hstr.Format(_T("%d"),widthheight%10000);
		temp = wstr + _T("x")+ hstr;
		//得到要修改成的width height
		TCHAR sz[50];
		GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(sz,50);
		width=_ttoi(sz);
		GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(sz,50);
		height=_ttoi(sz);
		if(width>0 && height> 0)//要修改的分辨率必须合法
		{
			wstr.Format(_T("%d"),width);
			hstr.Format(_T("%d"),height);
			update = wstr + _T("x")+ hstr;
			m_listRes.DeleteString(tmp);
			m_listRes.InsertString(tmp,update);
			m_listRes.SetItemData(tmp,width*10000+height);
			m_listRes.SetCurSel(tmp);
			//修改文件中的数值
			while(File.ReadString(strtemp))
			{
				if(strtemp.Find(temp)<0)//当前找不到List选中的要修改分辨率
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
		MessageBox(_T("Config.dat配置文件不在当前目录!"));
	}
}
void CYUVviewerPlusDlg::OnBnClickedButtonClear()
{
	// TODO: 在此添加控件通知处理程序代码
	OptionDlg dlg=new OptionDlg();
	if(dlg.DoModal()==IDOK)
	{ClearList();UpdateList();SetDefaultItem();}

	else
		SetDefaultItem();
}

void CYUVviewerPlusDlg::OnBnClickedButtonHelp()
{
	// TODO: 在此添加控件通知处理程序代码
	Help help=new Help();
	help.DoModal();
//	help.Create(IDD_HELP,NULL);
//	help.ShowWindow(SW_SHOW);
}

void CYUVviewerPlusDlg::OnBnClickedButtonOpen3()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CYUVviewerPlusDlg::OnDropFiles(HDROP hDropInfo)
{
	
	const int fileCount = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
	TCHAR fileName[MAX_PATH] = { 0 };
	CString fn;
	if(fileCount > 2)
	{
		MessageBox(_T("当前是2帧对比模式，只支持打开2个文件!"));
		return;
	}
	else if(fileCount == 1)//选择2帧对比模式，打开一个文件，
	{
		DragQueryFile(hDropInfo, 0, fileName, MAX_PATH);//获得文件路径
		fn.Format(_T("%s"),fileName);
		int index=fn.Find('.');
		CString name;
		name=fn.Right(fn.GetLength()-index-1);
		if(name.Find(_T("yuv"))<0)
		{
			MessageBox(_T("文件格式必须是.yuv！"));
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
			MessageBox(_T("只支持2个文件对比!"));
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
				MessageBox(_T("分辨率错误!"));
		}
	}
	CDialog::OnDropFiles(hDropInfo);
}

int CYUVviewerPlusDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO:  在此添加您专用的创建代码
	DragAcceptFiles(TRUE);
	return 0;
}
