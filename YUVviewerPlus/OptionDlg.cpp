// OptionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "YUVviewerPlus.h"
#include "OptionDlg.h"


// OptionDlg �Ի���

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
	CDialog::OnInitDialog();   // һ��Ҫ��
	m_list.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	m_list.SetView(LVS_REPORT);//�������������������listview��view��report�������û��Ҫ�ˣ�Ĭ�ϵ���icon
	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;    //����������fmtֵ����ʾ�б����ָ�����
	lvColumn.fmt = LVCFMT_LEFT;                     //����������ģʽ
	lvColumn.cx = 72;                                         //���ñ���Ŀ��Ϊ120������
	lvColumn.pszText = _T("   �ߴ�");             //���ñ��⣨������
	m_list.InsertColumn(0, &lvColumn);   //����У�����Ϊ0
	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 55;
	lvColumn.pszText = _T("���ȼ�");
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
			lvItem.iItem = colCount++;//�кţ���������0��ʼ���������൱�ڶ�ά�����ģʽ�����������ʱҪָ���кź��к�
			lvItem.iSubItem = 0;//�кţ���������0��ʼ��
			lvItem.pszText = (LPWSTR)(LPCTSTR)strtemp;//����
			nItem = m_list.InsertItem(&lvItem);//������һ�У������кţ�������
			File.ReadString(strtemp);
			m_list.SetItemText(nItem, 1, (LPWSTR)(LPCTSTR)strtemp);//�������������кţ������������кţ���ֵ
		}
		File.Close();
	}
	else
	{
		MessageBox(_T("Config.dat�����ļ����ڵ�ǰĿ¼!"));
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(OptionDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &OptionDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &OptionDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &OptionDlg::OnBnClickedButtonDel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST3, &OptionDlg::OnLvnItemchangedList3)
END_MESSAGE_MAP()


// OptionDlg ��Ϣ�������

void OptionDlg::OnBnClickedButtonSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�ȸ��ݷֱ��ʲ鿴���޸Ļ�������
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
				if(strtemp.Find(temp)<0)//δ�ҵ�
				{
					copy += _T("\r\n");
					copy += strtemp ;
				}
				else
				{
					File.ReadString(strtemp);
					find=true;
					
				}//�ҵ����������Ϊ��ɾ�����ȼ�
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
				strtemp = wstr + "x" + hstr;//�ֱ���
				lvItem.mask = LVIF_TEXT;
				lvItem.iItem = colCount++;//�кţ���������0��ʼ���������൱�ڶ�ά�����ģʽ�����������ʱҪָ���кź��к�
				lvItem.iSubItem = 0;//�кţ���������0��ʼ��
				lvItem.pszText = (LPWSTR)(LPCTSTR)strtemp;//����
				nItem = m_list.InsertItem(&lvItem);//������һ�У������кţ�������
				m_list.SetItemText(nItem, 1, (LPWSTR)(LPCTSTR)priostr);//�������������кţ������������кţ���ֵ
			}
			else//��LIST���ҵ��޸��� ����
			{
				temp = wstr + "x" + hstr;
				for(int i=0;i<m_list.GetItemCount();i++)
				{
					strtemp=m_list.GetItemText(i,0);
					if(strtemp.Find(temp)>=0)//�ҵ�
					{
						m_list.SetItemText(i, 1, priostr);
						break;
					}
				}
			}
		}
		else
		{
			MessageBox(_T("��ȸ߶�ֵ������������!"));
		}
	}
	else
	{
		MessageBox(_T("Config.dat�����ļ����ڵ�ǰĿ¼!"));
	}
}

void OptionDlg::OnBnClickedButtonClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_EDIT1_WIDTH)->SetWindowTextW(_T(""));
	GetDlgItem(IDC_EDIT1_HEIGHT)->SetWindowTextW(_T(""));
	GetDlgItem(IDC_EDIT1_PRIO)->SetWindowTextW(_T(""));
}

void OptionDlg::OnBnClickedButtonDel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int tmp=m_list.GetNextItem(-1,LVNI_SELECTED);
	if(tmp < 0)
		return;
	CString strtemp,wstr,hstr,temp,copy,tempPrio;
	int width,height,index;
	temp=m_list.GetItemText(tmp,0);//���ѡ���еķֱ���
	CStdioFile File;
	if(File.Open(_T(".\\Config.dat"),CFile::modeReadWrite))
	{
		//�õ�width height
		index=temp.Find('x');
		wstr=temp.Left(index);
		hstr=temp.Right(temp.GetLength()-index-1);
		if(hstr.GetAt(0)=='x')
			hstr.Delete(0,1);
		width=_ttoi(wstr);
		height=_ttoi(hstr);
		while(File.ReadString(strtemp))
		{
			if(strtemp.Find(temp)<0)//δ�ҵ�
			{copy += _T("\r\n");copy += strtemp ;}
			else
			{File.ReadString(strtemp);}//�ҵ����������Ϊ��ɾ�����ȼ�
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
		MessageBox(_T("Config.dat�����ļ����ڵ�ǰĿ¼!"));
		
	}
}

void OptionDlg::OnLvnItemchangedList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString wstr,hstr,priostr,temp;
	int index;
	int tmp=m_list.GetNextItem(-1,LVNI_SELECTED);
	temp=m_list.GetItemText(tmp,0);//���ѡ���еķֱ���
	index=temp.Find('x');
	wstr=temp.Left(index);
	hstr=temp.Right(temp.GetLength()-index-1);
	priostr=m_list.GetItemText(tmp,1);
	GetDlgItem(IDC_EDIT1_WIDTH)->SetWindowTextW((LPWSTR)(LPCTSTR)wstr);
	GetDlgItem(IDC_EDIT1_HEIGHT)->SetWindowTextW((LPWSTR)(LPCTSTR)hstr);
	GetDlgItem(IDC_EDIT1_PRIO)->SetWindowTextW((LPWSTR)(LPCTSTR)priostr);
	*pResult = 0;
}
