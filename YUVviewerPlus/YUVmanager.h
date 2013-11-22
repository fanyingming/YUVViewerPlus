#pragma once
#ifndef YUVMANAGER
#define YUVMANAGER
#include "ShowDlg.h"
#include "PanDialog.h"
#include "emmintrin.h"
extern void yv12_to_rgb32(unsigned char * rgb32, int stride, unsigned char * yuv, int width, int height);
extern void yv12_diff_to_rgb32(unsigned char * rgb32, int stride, unsigned char * yuv1, unsigned char * yuv2, int width, int height) ;
#define BUFFER_POOL_SIZE 20
#define YUV_MAX_SIZE (1920*1080*BUFFER_POOL_SIZE*3/2)
#define PIC_SIZE ((m_width*m_height*3)>>1)
#define BUFFER_LOWER_BOUND_FORWARD	3
#define BUFFER_LOWER_BOUND_BACKWARD	2
#define BUFFER_UPPER_BOUND_FORWARD	6
#define BUFFER_UPPER_BOUND_BACKWARD	5


template <class T>
class ThreadEntryInfo
{
public:
	T *obj;
	void (T::*method)(void);

	static void ThreadEntry(void *ptr)
	{
		ThreadEntryInfo info=*(ThreadEntryInfo*)ptr;
		(info.obj->*(info.method))();
	}
};

class YUVOperator
{
public:
	YUVOperator(void)
	{
		m_buffer=(char*)align_malloc(YUV_MAX_SIZE);
		m_RGBbuffer=(char*)align_malloc(1920*1080*4);
		m_YUVbuffer=(char*)align_malloc(1920*1080*3/2);
		m_width=m_height=m_buffersize=-1;
		m_head=m_tail=m_curframe=m_total=-1;
		m_pointer=NULL;
		m_mutex=CreateMutex(NULL,0,NULL);
		m_readingthread=NULL;
		m_state=0;
		m_file=NULL;
		m_contrast=100;
		contentsID=-1;
	}

	~YUVOperator(void)
	{
		Clean();
		align_free(m_buffer);
		align_free(m_RGBbuffer);
		align_free(m_YUVbuffer);
		CloseHandle(m_mutex);
	}
	void CreateNewSeq(int width,int height,CString name)
	{
		Clean();
		m_width=width;
		m_height=height;
		m_name=name;
		m_buffersize=BUFFER_POOL_SIZE*PIC_SIZE;
		Setup();
	}
	bool Next()
	{
		char *tmp=GetNextPointer();
		if(!tmp)
			return 1;
		return 0;
	}
	bool Prev()
	{
		char *tmp=GetPrevPointer();
		if(!tmp)
			return 1;
		return 0;
	}
	void ContrastUp()
	{
		m_contrast+=10;
		if(m_contrast>200)
			m_contrast=200;
	}
	void ContrastDown()
	{
		m_contrast-=10;
		if(m_contrast<0)
			m_contrast=0;
	}
	bool ReachHeadBound()
	{
		if(m_curframe<=0)
			return 1;
		return 0;
	}
	bool ReachTailBound()
	{
		if(m_curframe>=m_total-1)
			return 1;
		return 0;
	}
	//从yuv中得到RGB，并更新窗口信息
	void GetImage(DisplayInfo *info,char *RGBbuffer)
	{
		if(info)
		{
			info->contrast=m_contrast;
			info->curframe=m_curframe;
			info->filename=m_name;
		}
		if(RGBbuffer)
		{
			memcpy(m_YUVbuffer,m_pointer,PIC_SIZE);
		//	PostProc();
			yv12_to_rgb32((unsigned char*)m_RGBbuffer,m_width*4,(unsigned char*)m_YUVbuffer,m_width,m_height);
			memcpy(RGBbuffer,m_RGBbuffer,m_width*m_height*4);
		}
	}

	void GetDiffImage(DisplayInfo *info,char *RGBbuffer,char* buffer1,char* buffer2,int CurrentFrame)
	{
		if(info)
		{
			info->contrast=m_contrast;
			info->curframe=CurrentFrame;
			info->filename="Diff";
		}
		if(RGBbuffer)
		{
		//	PostProc();
			yv12_diff_to_rgb32((unsigned char*)m_RGBbuffer,m_width*4,(unsigned char*)buffer1,(unsigned char*)buffer2,m_width,m_height);
			memcpy(RGBbuffer,m_RGBbuffer,m_width*m_height*4);
		}
	}
	void BackToZero()
	{
		Setup();
	}
public:
	char* GetNextPointer()
	{
		if(m_curframe>=m_total-1)
			return NULL;
		while(GetTail()<=m_curframe)
			Sleep(1);
		char *temp=m_pointer;
		temp+=PIC_SIZE;
		WaitForSingleObject(m_mutex,INFINITE);
		m_pointer=FoldPointer(temp);
		m_curframe++;
		ReleaseMutex(m_mutex);
		return m_pointer;
	}
	char* GetPrevPointer()
	{
		if(m_curframe<=0)
			return NULL;
		while(GetHead()>=m_curframe)
			Sleep(1);
		char *temp=m_pointer;
		temp-=PIC_SIZE;
		WaitForSingleObject(m_mutex,INFINITE);
		m_pointer=FoldPointer(temp);
		m_curframe--;
		ReleaseMutex(m_mutex);
		return m_pointer;
	}
	int GetHead()
	{
		WaitForSingleObject(m_mutex,INFINITE);
		int temp=m_head;
		ReleaseMutex(m_mutex);
		return temp;
	}
	int GetTail()
	{
		WaitForSingleObject(m_mutex,INFINITE);
		int temp=m_tail;
		ReleaseMutex(m_mutex);
		return temp;
	}
	void ReadBuffer()
	{
		int obj_tail,obj_head,need_readtail,need_readhead;
		char *tail_pointer,*head_pointer;
		while(m_state)
		{
			
			//check if we need to read some data
			WaitForSingleObject(m_mutex,INFINITE);
			obj_tail=min(m_curframe+BUFFER_LOWER_BOUND_FORWARD,m_total);
			if(m_tail<obj_tail)
			{
				obj_tail=min(m_curframe+BUFFER_UPPER_BOUND_FORWARD,m_total);
				need_readtail=obj_tail-m_tail;
				tail_pointer=m_pointer+PIC_SIZE*(m_tail-m_curframe+1);
			}
			else
			{
				obj_tail=min(m_curframe+BUFFER_UPPER_BOUND_FORWARD,m_total);
				need_readtail=0;
			}
			obj_head=max(m_curframe-BUFFER_LOWER_BOUND_BACKWARD,0);
			if(m_head>obj_head)
			{
				obj_head=max(m_curframe-BUFFER_UPPER_BOUND_BACKWARD,0);
				need_readhead=m_head-obj_head;
				head_pointer=m_pointer-PIC_SIZE*(m_curframe-m_head+need_readhead);
			}
			else
			{
				obj_head=max(m_curframe-BUFFER_UPPER_BOUND_BACKWARD,0);
				need_readhead=0;
			}
			ReleaseMutex(m_mutex);

			//async reading
			if(need_readtail)
			{
				int size=need_readtail*PIC_SIZE;
				DWORD temp;
				tail_pointer=FoldPointer(tail_pointer);
				SetFilePointer(m_file,(m_tail/*+need_readtail*/)*PIC_SIZE,0,FILE_BEGIN);
				if(tail_pointer+size>m_buffersize+m_buffer)//if need folding
				{
					ReadFile(m_file,tail_pointer,m_buffersize+m_buffer-tail_pointer,&temp,NULL);
					ReadFile(m_file,m_buffer,size-(m_buffersize+m_buffer-tail_pointer),&temp,NULL);
				}
				else
					ReadFile(m_file,tail_pointer,size,&temp,NULL);
			}

			if(need_readhead)
			{
				int size=need_readhead*PIC_SIZE;
				DWORD temp;
				SetFilePointer(m_file,(m_head-need_readhead)*PIC_SIZE,0,FILE_BEGIN);
				if(head_pointer<m_buffer)//if need folding
				{
					int foldsize;
					head_pointer=FoldPointer(head_pointer);
					ReadFile(m_file,head_pointer,foldsize=m_buffersize+m_buffer-head_pointer,&temp,NULL);
					ReadFile(m_file,m_buffer,size-foldsize,&temp,NULL);
				}
				else
					ReadFile(m_file,head_pointer,size,&temp,NULL);
			}
			if(need_readhead|need_readtail)
			{
				WaitForSingleObject(m_mutex,INFINITE);
				m_head=obj_head;
				ReleaseMutex(m_mutex);
				WaitForSingleObject(m_mutex,INFINITE);
				m_tail=obj_tail;
				ReleaseMutex(m_mutex);
			}
			Sleep(5);

		}
	}

	char* FoldPointer(char *src)
	{
		if(!src)
			return src;
		if(src>=m_buffer+m_buffersize)
			src-=m_buffersize;
		if(src<m_buffer)
			src+=m_buffersize;
		return src;
	}
	void PostProc()
	{
		_mm_empty();
		int i,j;
		__m128i coeff=_mm_set1_epi16(m_contrast*8192/100),zero=_mm_setzero_si128(),mid=_mm_set1_epi16(128),buf1,buf2;
		for(i=0;i<m_height;i++)
		{
			for(j=0;j<m_width;j+=16)
			{	
				__m128i *pix;
				pix=(__m128i *)(m_YUVbuffer+i*m_width+j);
				buf1=_mm_unpacklo_epi8(*pix,zero);
				buf1=_mm_sub_epi16(buf1,mid);
				buf1=_mm_slli_epi16(buf1,3);
				buf1=_mm_mulhi_epi16(buf1,coeff);
				buf1=_mm_add_epi16(buf1,mid);
				buf2=_mm_unpackhi_epi8(*pix,zero);
				buf2=_mm_sub_epi16(buf2,mid);
				buf2=_mm_slli_epi16(buf2,3);
				buf2=_mm_mulhi_epi16(buf2,coeff);
				buf2=_mm_add_epi16(buf2,mid);

				*pix=_mm_packus_epi16(buf1,buf2);
			}
		}
	}
	void Clean()
	{
		m_state=0;
		if(m_readingthread)
		{
			WaitForSingleObject(m_readingthread,INFINITE);
			CloseHandle(m_readingthread);
			m_readingthread=NULL;
		}
		if(m_file)
		{
			CloseHandle(m_file);
			m_file=NULL;
		}
		m_pointer=m_buffer;
		m_head=m_tail=-1;
		m_curframe=0;
		m_contrast=100;

	}
	void CleanDiff()
	{
		m_state=0;

		if(m_file)
		{
			CloseHandle(m_file);
			m_file=NULL;
		}
		m_pointer=m_buffer;
		m_head=m_tail=-1;
		m_curframe=0;
		m_contrast=100;

	}
	void Setup()
	{
		Clean();
		m_file=CreateFile(m_name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL);
		if(!m_file)
			return;
		{//read frame 0 and the filesize
			DWORD temp;
			ReadFile(m_file,m_buffer,PIC_SIZE,&temp,NULL);
			m_head=0;
			m_tail=0;
			LARGE_INTEGER fileSize;
			GetFileSizeEx(m_file,&fileSize);
			m_total=fileSize.QuadPart/PIC_SIZE;//支持大文件；m_total应该是总帧数
		}

		{//begin buffer-reading thread
			m_state=1;
			m_threadinfo.obj=this;
			m_threadinfo.method=&YUVOperator::ReadBuffer;
			m_readingthread=(HANDLE)_beginthread(&ThreadEntryInfo<YUVOperator>::ThreadEntry,0,&m_threadinfo);
		}
	}
public:
	void GetYUVBuffer(char* buffer)
	{
		memcpy(buffer,m_YUVbuffer,PIC_SIZE);
	}
	int GetCurrentFrame()
	{
		return m_curframe;
	}
	void SetWidthHeight(int width,int height)
	{
		m_width=width;
		m_height=height;
	}
private:

	char *m_buffer,*m_pointer;
	char *m_RGBbuffer,*m_YUVbuffer;
	int m_width,m_height,m_buffersize;
	int m_head,m_tail,m_curframe,m_total;
	int m_state;
	int m_contrast;
	int contentsID;
	HANDLE m_mutex;
	HANDLE m_readingthread;
	HANDLE m_file;
	CString m_name;
	ThreadEntryInfo<YUVOperator> m_threadinfo;
};

enum DisplayMode{
	DM_SINGLE=0,
	DM_HOR,
	DM_VER,
	DM_CUBE,
};
class YUVManager
{
public:
	YUVManager()
	{
		m_context[0]=m_context[1]=m_context[2]=m_context[3]=NULL;
		m_shDlg=NULL;
		m_panDlg=NULL;
		m_height=m_width=0;
		m_curContext=0;
		m_RGBbuffer=(char*)align_malloc(1920*1080*4);
		m_YUVbuffer1=(char*)align_malloc(1920*1080*3/2);
		m_YUVbuffer2=(char*)align_malloc(1920*1080*3/2);
//		m_RGBtempbuffer=(char*)align_malloc(1920*1080);
		m_mutex=CreateMutex(NULL,0,NULL);
		m_displaymode=DM_SINGLE;
		m_link[0]=m_link[1]=m_link[2]=NULL;
		linkFlag=false;
	}
	~YUVManager()
	{
		Clean();
		align_free(m_RGBbuffer);
		align_free(m_YUVbuffer1);
		align_free(m_YUVbuffer2);
//		align_free(m_RGBtempbuffer);
		CloseHandle(m_mutex);
	}
	void Reset()
	{
	//	WaitForSingleObject(m_mutex,INFINITE);
		Clean();
	}

	void CreateNewSeq1(int width,int height,CString name,int contextID)
	{
		WaitForSingleObject(m_mutex,INFINITE);
		if(!m_context[contextID])
		{
			m_context[contextID]=new YUVOperator;
		}
		m_context[contextID]->CreateNewSeq(width,height,name);
		m_width=width;
		m_height=height;
		m_curContext=contextID;
		m_displaymode=DM_SINGLE;
		Show();
		Next();
		Prev();
		ReleaseMutex(m_mutex);
	}
	void FullScreen(bool fullscreen)
	{
		WaitForSingleObject(m_mutex,INFINITE);
		if(fullscreen)
		{
			if(m_shDlg)
			{
				CRect rect,rect2;
				m_panDlg->GetClientRect(&rect);
				m_panDlg->GetWindowRect(&rect2);
				m_panDlg->ClientToScreen(rect);
				m_panDlg->SetWindowPos(NULL,0,0,0,0,SWP_NOSIZE);
				m_panDlg->ModifyStyle(WS_CAPTION,0,0);
				m_panDlg->ShowWindow(SW_SHOWMAXIMIZED);
			}
		}
		else
		{
			if(m_panDlg)
			{
				m_panDlg->ModifyStyle(0,WS_CAPTION,0);

				m_panDlg->ShowWindow(SW_SHOWNORMAL);
			}
		}
		ReleaseMutex(m_mutex);

	}
	bool SetDisplayMode(DisplayMode mode)
	{
		WaitForSingleObject(m_mutex,INFINITE);
		bool fail=0;
		int temp=0;
		switch(mode)
		{
		case DM_SINGLE:
			m_displaymode=DM_SINGLE;
			break;
		case DM_HOR:
			{
				int i;
				for(i=0;i<4;i++)
					if(m_context[i])
						temp+=m_width;
				if(temp>1920)
					fail=1;
				else
					m_displaymode=DM_HOR;
				break;
			}
		case DM_VER:
			{
				int i;
				for(i=0;i<4;i++)
					if(m_context[i])
						temp+=m_height;
				if(temp>1080)
					fail=1;
				else
					m_displaymode=DM_VER;
				break;
			}
		case DM_CUBE:
			{
				int i;
				for(i=0;i<4;i++)
					if(m_context[i])
						temp+=1;
				if(temp!=4)
					fail=1;
				if(m_height*2>1080||m_width*2>1920)
					fail=1;
				if(!fail)
					m_displaymode=DM_CUBE;

				break;
			}
		}
		Show();
		ReleaseMutex(m_mutex);
		return fail;
	}

	bool Next()
	{
		WaitForSingleObject(m_mutex,INFINITE);
		int i;
		bool stop=0;
		for(i=0;i<4;i++)
			if(m_context[i])
				stop=stop||m_context[i]->ReachTailBound();
		if(!stop)
		{
			for(i=0;i<4;i++)
				if(m_context[i])
					m_context[i]->Next();
			Show();
		}
		ReleaseMutex(m_mutex);
		return stop;
	}

	bool Prev()
	{
		WaitForSingleObject(m_mutex,INFINITE);
		int i;
		bool stop=0;
		for(i=0;i<4;i++)
			if(m_context[i])
				stop=stop||m_context[i]->ReachHeadBound();
		if(!stop)
		{
			for(i=0;i<4;i++)
				if(m_context[i])
					m_context[i]->Prev();
			Show();
		}
		ReleaseMutex(m_mutex);
		return stop;
	}
	void SetMainWnd(CWnd *mainWnd)
	{
		WaitForSingleObject(m_mutex,INFINITE);
		p_mainWnd=mainWnd;
		ReleaseMutex(m_mutex);
	}
	void ContrastUp()
	{
		WaitForSingleObject(m_mutex,INFINITE);
		/*int i;
			for(i=0;i<4;i++)
		if(m_context[i])
		m_context[i]->ContrastUp();
		*/ 
		if(m_context[m_curContext])
			m_context[m_curContext]->ContrastUp();
		Show();
		ReleaseMutex(m_mutex);
	}
	void ContrastDown()
	{
		WaitForSingleObject(m_mutex,INFINITE);
	/*	int i;
		
		for(i=0;i<4;i++)
		if(m_context[i])
		m_context[i]->ContrastDown();
		*/
		if(m_context[m_curContext])
			m_context[m_curContext]->ContrastDown();
		Show();
		ReleaseMutex(m_mutex);
	}
	void SwitchContext(int ContextID)
	{
		WaitForSingleObject(m_mutex,INFINITE);
		if(m_context[ContextID])
		{
			m_curContext=ContextID;
			Show();
		}
		ReleaseMutex(m_mutex);
	}
	void BackToZero()
	{
		WaitForSingleObject(m_mutex,INFINITE);
		int i;
		for(i=0;i<4;i++)
			if(m_context[i])
				m_context[i]->BackToZero();
		Show();
		ReleaseMutex(m_mutex);
	}



public:
	void Show()
	{
		if(!m_panDlg)
		{//init PanDlg
			m_panDlg=new PanDialog();
			m_panDlg->SetCurContext(m_curContext);
			m_panDlg->Create(IDD_DIALOG_PAN);
			m_panDlg->ShowWindow(1);
			CRect wndRect,cliRect;
			m_panDlg->GetWindowRect(&wndRect);
			m_panDlg->GetClientRect(&cliRect);
			m_panDlg->SetWindowPos(NULL,0,0,m_width+wndRect.Width()-cliRect.Width(),
				m_height+wndRect.Height()-cliRect.Height(),SWP_NOMOVE);
		}
		if(!m_shDlg)
		{//init ShowDlg
			m_shDlg=new ShowDlg();
			m_shDlg->SetCurContext(m_curContext);
			m_shDlg->Create(IDD_SHOWDLG);
			m_shDlg->SetParent(m_panDlg);
			m_shDlg->SetIsSame(false);

		}
		
		DisplayInfo info;
		if(m_displaymode==DM_SINGLE)
		{
			if(m_context[m_curContext])
			{
				m_context[m_curContext]->GetImage(&info,m_RGBbuffer);

				info.contextid=m_curContext;
				m_shDlg->SetDisplayInfo(info);
			}
		}
			
			if(m_context[0])
			{
				m_context[0]->GetYUVBuffer(m_YUVbuffer1);
				m_shDlg->GetYUVBuffer(m_YUVbuffer1,m_YUVbuffer1);
			}
			else if(m_context[1])
			{
				m_context[1]->GetYUVBuffer(m_YUVbuffer2);
				m_shDlg->GetYUVBuffer(m_YUVbuffer2,m_YUVbuffer2);
			}
			
			/////////////
		/*
		else
		{
		int i,j,k;
		for(i=0,k=0;i<4;i++)
		{
		if(m_context[i])
		k++;
		}
		for(i=0,k=0,j=0;i<4;i++)
		{
		if(m_context[i])
		{
		m_context[i]->GetImage(&info,m_RGBbuffer+k);
		info.contextid=i;
		m_shDlg[j++]->SetDisplayInfo(info);
		}
		k+=m_width*m_height*4;
		}
		}
		*/
		//		int width=m_width,height=m_height;
		switch(m_displaymode)
		{
		case DM_SINGLE:
			{
				m_shDlg->ShowWindow(1);
				m_shDlg->SetWindowPos(NULL,0,0,0,0,SWP_NOSIZE);
				m_shDlg->SetWH(m_width,m_height);
				m_shDlg->SetBitmap(m_RGBbuffer);
				break;
			}
			/*
			case DM_HOR:
			{
			int i,j,k;
			for(i=0,k=0;i<4;i++)
			{
			if(m_context[i])
			{
			m_shDlg[k]->ShowWindow(1);
			m_shDlg[k]->SetWindowPos(NULL,k*m_width,0,0,0,SWP_NOSIZE);
			m_shDlg[k]->SetWH(m_width,m_height);
			m_shDlg[k]->SetBitmap(m_RGBbuffer+k*m_width*m_height*4);
			k++;
			}
			}
			width=k*m_width;
			break;
			}
			case DM_VER:
			{
			int i,j,k;
			for(i=0,k=0;i<4;i++)
			{
			if(m_context[i])
			{
			m_shDlg[k]->ShowWindow(1);
			m_shDlg[k]->SetWindowPos(NULL,0,k*m_height,0,0,SWP_NOSIZE);
			m_shDlg[k]->SetWH(m_width,m_height);
			m_shDlg[k]->SetBitmap(m_RGBbuffer+k*m_width*m_height*4);
			k++;
			}
			}
			height=k*m_height;
			break;
			}
			case DM_CUBE:
			{
			int i,j,k;
			for(i=0,k=0;i<4;i++)
			{
			m_shDlg[i]->ShowWindow(1);
			m_shDlg[i]->SetWindowPos(NULL,(i%2)*m_width,(i/2)*m_height,0,0,SWP_NOSIZE);
			m_shDlg[i]->SetWH(m_width,m_height);
			m_shDlg[i]->SetBitmap(m_RGBbuffer+i*m_width*m_height*4);
			}
			height=2*m_height;
			width=2*m_width;
			break;
			}
			*/
		}
	}

	void Clean()
	{
		int i;
		for(i=0;i<4;i++)
		{
			if(m_context[i])
			{
				delete m_context[i];
				m_context[i]=NULL;
			}
		}
		if(m_shDlg)
			{
				m_shDlg->DestroyWindow();
				delete m_shDlg;
				m_shDlg=NULL;
			}
		if(m_panDlg)
		{
			m_panDlg->DestroyWindow();
			delete m_panDlg;
			m_panDlg=NULL;
		}
	}

	void GetYUVOperator1YUVBuffer( char* buffer)
	{
		if(m_context[0])
			m_context[0]->GetYUVBuffer(buffer);
	}

	void GetYUVOperator2YUVBuffer( char* buffer)
	{
		if(m_context[1])
			m_context[1]->GetYUVBuffer(buffer);
	}
	bool IsSame()
	{
		int i,j;
		char* y1 = m_YUVbuffer1;
		char* u1 = y1 + m_width * m_height;
		char* v1 = u1 + m_width * m_height / 4;
		char* y2 = m_YUVbuffer2;
		char* u2 = y2 + m_width * m_height;
		char* v2 = u2 + m_width * m_height / 4;
		for(i=0;i<m_height;i++)
		{
			for(j=0;j<m_width;j++)
			{
				int cy1 = y1[j];
				int cu1 = u1[j / 2];
				int cv1 = v1[j / 2];
				int cy2 = y2[j];
				int cu2 = u2[j / 2];
				int cv2 = v2[j / 2];
				if(cy1 != cy2 || cu1 != cu2 || cv1 !=cv2)
					return false;
			}
			y1 += m_width;
			y2 += m_width;
			if ( (i & 0x1) != 0 )
			{
				u1 += m_width / 2;
				v1 += m_width / 2;
				u2 += m_width / 2;
				v2 += m_width / 2;
			}
		}
		return true;
	}
	int ShowDiff(int CurrentFrame)
	{
		if(!m_panDlg)
		{//init PanDlg
			m_panDlg=new PanDialog();
			m_panDlg->SetCurContext(2);
			m_panDlg->Create(IDD_DIALOG_PAN);
			m_panDlg->ShowWindow(1);
			CRect wndRect,cliRect;
			m_panDlg->GetWindowRect(&wndRect);
			m_panDlg->GetClientRect(&cliRect);
			m_panDlg->SetWindowPos(NULL,0,0,m_width+wndRect.Width()-cliRect.Width(),
				m_height+wndRect.Height()-cliRect.Height(),SWP_NOMOVE);
		}
		if(!m_shDlg)
		{//init ShowDlg
			m_shDlg=new ShowDlg();
			m_shDlg->SetCurContext(2);
			m_shDlg->Create(IDD_SHOWDLG);
			m_shDlg->SetParent(m_panDlg);
		}
		DisplayInfo info;
		if(IsSame())//如果俩帧图像完全相同。
		{
			info.curframe=CurrentFrame;
			if(m_context[0])
			{
				m_shDlg->SetDisplayInfo(info);
				m_shDlg->ShowWindow(1);
				m_shDlg->SetWindowPos(NULL,0,0,0,0,SWP_NOSIZE);
				m_shDlg->SetWH(m_width,m_height);
				m_shDlg->SetIsSame(true);
				m_shDlg->ShowSame();
			}
			return 0;
		}
		m_shDlg->SetIsSame(false);
		if(m_displaymode==DM_SINGLE)
		{
			if(m_context[0])
			{
				m_context[0]->GetDiffImage(&info,m_RGBbuffer,m_YUVbuffer1,m_YUVbuffer2,CurrentFrame);
				m_shDlg->GetYUVBuffer(m_YUVbuffer1,m_YUVbuffer2);
				//	info.contextid=0;
				m_shDlg->SetDisplayInfo(info);
			}
		}
		switch(m_displaymode)
		{
		case DM_SINGLE:
			{
				m_shDlg->ShowWindow(1);
				m_shDlg->SetWindowPos(NULL,0,0,0,0,SWP_NOSIZE);
				m_shDlg->SetWH(m_width,m_height);
				m_shDlg->SetBitmap(m_RGBbuffer);
				break;
			}
		}
		return 1;
	}
	void Init(int width,int height,int CurrentFrame)
	{
		WaitForSingleObject(m_mutex,INFINITE);
		if(!m_context[0])
		{
			m_context[0]=new YUVOperator;
			m_context[0]->SetWidthHeight(width,height);
		}
		m_width=width;
		m_height=height;
		m_displaymode=DM_SINGLE;
		ReleaseMutex(m_mutex);
	}
	int GetCurrentFrame()
	{
		if(m_context[0])
			return m_context[0]->GetCurrentFrame();
		return -1;
	}

	void SetLink(ShowDlg *p[])
	{
		m_link[0]=p[0];
		m_link[1]=p[1];
		m_link[2]=p[2];
	}
	ShowDlg* GetLink()
	{
		return m_shDlg;
	}
	int m_width,m_height;
	int m_curContext;
	DisplayMode m_displaymode;
	char *m_RGBbuffer,*m_RGBtempbuffer,*m_YUVbuffer1,*m_YUVbuffer2;
	YUVOperator *m_context[4];

	PanDialog *m_panDlg;
	CWnd *p_mainWnd;
	HANDLE m_mutex;
	ShowDlg *m_link[3];
	YUVManager *mager1,*mager2;
public:
	bool linkFlag;
	ShowDlg *m_shDlg;
};


#endif