// OptionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "YUVviewerPlus.h"
#include "OptionDlg.h"


// OptionDlg 对话框

IMPLEMENT_DYNAMIC(OptionDlg, CDialog)

OptionDlg::OptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(OptionDlg::IDD, pParent)
{
	colCount=0;

}

OptionDlg::~OptionDlg()
{
}

void OptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, m_list);
}
BOOL OptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();   // 一定要加
	m_list.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	m_list.SetView(LVS_REPORT);//如果在属性里面设置了listview的view是report，这里就没必要了，默认的是icon
	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;    //掩码设置了fmt值、显示列标题和指定宽度
	lvColumn.fmt = LVCFMT_LEFT;                     //设置作对其模式
	lvColumn.cx = 72;                                         //设置标题的宽度为120个像素
	lvColumn.pszText = _T("   尺寸");             //设置标题（列名）
	m_list.InsertColumn(0, &lvColumn);   //添加列，索引为0
	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 55;
	lvColumn.pszText = _T("优先级");
	m_list.InsertColumn(1, &lvColumn);
	LVITEM lvItem;
	int nItem;
	CStdioFile File;
	if(File.Open(_T(".\\Config.dat"),CFile::modeRead))
	{
		CString str=_T(""),strtemp=_T("");
		while(File.ReadString(strtemp))
		{
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = colCount++;//行号（索引，从0开始），报表相当于二维数组的模式，添加行内容时要指定行号和列号
			lvItem.iSubItem = 0;//列号（索引，从0开始）
			lvItem.pszText = (LPWSTR)(LPCTSTR)strtemp;//内容
			nItem = m_list.InsertItem(&lvItem);//插入这一行，返回行号（索引）
			File.ReadString(strtemp);
			m_list.SetItemText(nItem, 1, (LPWSTR)(LPCTSTR)strtemp);//设置行索引（行号）和列索引（列号）的值
		}
		File.Close();
	}
	else
	{
		MessageBox(_T("Config.dat配置文件不在当前目录!"));
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(OptionDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &OptionDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &OptionDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &OptionDlg::OnBnClickedButtonDel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST3, &OptionDlg::OnLvnItemchangedList3)
END_MESSAGE_MAP()


// OptionDlg 消息处理程序

void OptionDlg::OnBnClickedButtonSave()
{
	// TODO: 在此添加控件通知处理程序代码
	//先根据分辨率查看是修改还是新增
	CString strtemp,wstr,hstr,priostr,temp,copy;
	int width,height,prio;
	TCHAR sz[50];
	CStdioFile File;
	LVITEM lvItem;
	int nItem;
	GetDlgItem(IDC_EDIT1_WIDTH)->GetWindowText(sz,50);
	width=_ttoi(sz);
	wstr=sz;
	GetDlgItem(IDC_EDIT1_HEIGHT)->GetWindowText(sz,50);
	height=_ttoi(sz);
	hstr=sz;
	GetDlgItem(IDC_EDIT1_PRIO)->GetWindowText(sz,50);
	prio=_ttoi(sz);
	priostr=sz;
	temp = wstr + "x" + hstr; 
	bool find=false;
	if(File.Open(_T(".\\Config.dat"),CFile::modeReadWrite))
	{
		if(width>0 && height> 0)
		{
			while(File.ReadString(strtemp))
			{
				if(strtemp.Find(temp)<0)//未找到
				{
					copy += _T("\r\n");
					copy += strtemp ;
				}
				else
				{
					File.ReadString(strtemp);
					find=true;
					
				}//找到后继续下移为了删掉优先级
			}
			File.Close();
			File.Open(_T(".\\Config.dat"),CFile::modeCreate|CFile::modeReadWrite);
			strtemp="\r\n";
			strtemp += wstr + "x" + hstr + "\r\n" + priostr;
			copy += strtemp;
			copy.Delete(0,2);
			File.WriteString(copy);
			File.Close();
			if(!find)
			{
				strtemp = wstr + "x" + hstr;//分辨率
				lvItem.mask = LVIF_TEXT;
				lvItem.iItem = colCount++;//行号（索引，从0开始），报表相当于二维数组的模式，添加行内容时要指定行号和列号
				lvItem.iSubItem = 0;//列号（索引，从0开始）
				lvItem.pszText = (LPWSTR)(LPCTSTR)strtemp;//内容
				nItem = m_list.InsertItem(&lvItem);//插入这一行，返回行号（索引）
				m_list.SetItemText(nItem, 1, (LPWSTR)(LPCTSTR)priostr);//设置行索引（行号）和列索引（列号）的值
			}
			else//在LIST中找到修改项 更新
			{
				temp = wstr + "x" + hstr;
				for(int i=0;i<m_list.GetItemCount();i++)
				{
					strtemp=m_list.GetItemText(i,0);
					if(strtemp.Find(temp)>=0)//找到
					{
						m_list.SetItemText(i, 1, priostr);
						break;
					}
				}
			}
		}
		else
		{
			MessageBox(_T("宽度高度值必须是正整数!"));
		}
	}
	else
	{
		MessageBox(_T("Config.dat配置文件不在当前目录!"));
	}
}

void OptionDlg::OnBnClickedButtonClear()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_EDIT1_WIDTH)->SetWindowTextW(_T(""));
	GetDlgItem(IDC_EDIT1_HEIGHT)->SetWindowTextW(_T(""));
	GetDlgItem(IDC_EDIT1_PRIO)->SetWindowTextW(_T(""));
}

void OptionDlg::OnBnClickedButtonDel()
{
	// TODO: 在此添加控件通知处理程序代码
	int tmp=m_list.GetNextItem(-1,LVNI_SELECTED);
	if(tmp < 0)
		return;
	CString strtemp,wstr,hstr,temp,copy,tempPrio;
	int width,height,index;
	temp=m_list.GetItemText(tmp,0);//获得选中行的分辨率
	CStdioFile File;
	if(File.Open(_T(".\\Config.dat"),CFile::modeReadWrite))
	{
		//得到width height
		index=temp.Find('x');
		wstr=temp.Left(index);
		hstr=temp.Right(temp.GetLength()-index-1);
		if(hstr.GetAt(0)=='x')
			hstr.Delete(0,1);
		width=_ttoi(wstr);
		height=_ttoi(hstr);
		while(File.ReadString(strtemp))
		{
			if(strtemp.Find(temp)<0)//未找到
			{copy += _T("\r\n");copy += strtemp ;}
			else
			{File.ReadString(strtemp);}//找到后继续下移为了删掉优先级
		}
		File.Close();
		File.Open(_T(".\\Config.dat"),CFile::modeCreate|CFile::modeReadWrite);
		copy.Delete(0,2);
		File.WriteString(copy);
		File.Close();
		GetDlgItem(IDC_EDIT1_WIDTH)->SetWindowTextW(_T(""));
		GetDlgItem(IDC_EDIT1_HEIGHT)->SetWindowTextW(_T(""));
		GetDlgItem(IDC_EDIT1_PRIO)->SetWindowTextW(_T(""));
		m_list.DeleteItem(tmp);
	}
	else
	{
		MessageBox(_T("Config.dat配置文件不在当前目录!"));
		
	}
}

void OptionDlg::OnLvnItemchangedList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CString wstr,hstr,priostr,temp;
	int index;
	int tmp=m_list.GetNextItem(-1,LVNI_SELECTED);
	temp=m_list.GetItemText(tmp,0);//获得选中行的分辨率
	index=temp.Find('x');
	wstr=temp.Left(index);
	hstr=temp.Right(temp.GetLength()-index-1);
	priostr=m_list.GetItemText(tmp,1);
	GetDlgItem(IDC_EDIT1_WIDTH)->SetWindowTextW((LPWSTR)(LPCTSTR)wstr);
	GetDlgItem(IDC_EDIT1_HEIGHT)->SetWindowTextW((LPWSTR)(LPCTSTR)hstr);
	GetDlgItem(IDC_EDIT1_PRIO)->SetWindowTextW((LPWSTR)(LPCTSTR)priostr);
	*pResult = 0;
}
