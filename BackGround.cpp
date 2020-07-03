#include "stdafx.h"
#include <direct.h>
#include "resource.h"
#include "BackGround.h"

CBackGround::CBackGround(int width, int height)
	:CLayer(width,height)
{
	m_pic=NULL;
	m_buffer=NULL;
	ImageFromIDResource(IDB_GRASS,"PNG",m_defaultbg);
}


CBackGround::~CBackGround(void)
{
	if(m_buffer)delete m_buffer;
	if(m_defaultbg) delete m_defaultbg;
}


void CBackGround::FreeRes(){
	if(m_pic){
		delete m_pic;
		m_pic=NULL;
	}
	if(m_buffer){
		delete m_buffer;
		m_buffer = NULL;
	}
}

void CBackGround::SetRes( string url ){
	m_url = url;
}

void CBackGround::ReadRes(){
	USES_CONVERSION;
	if(m_pic){
		delete m_pic;
		m_pic=NULL;
	}

	try{
		m_pic = Bitmap::FromFile(A2W(m_url.c_str()));
		if(m_pic->GetLastStatus()==S_OK){
			CLayer::setWWH(m_pic->GetWidth(), m_pic->GetHeight());		
		}
		else{
			MessageBox(NULL,"ͼƬ����ʧ�ܣ��˵�-������/������Դ","��Դ����",NULL);
		}
	}
	catch(...){
		m_buffer = NULL;
		char msg[1024];
		sprintf_s(msg,1024,"��ϵͳ�ڴ�̫С���޷�������Դ�ļ�");
		MessageBox(NULL,msg,"�ڴ治��",NULL);
	}
	ScaleXY();
}

void CBackGround::ScaleXY(){
	if(m_buffer){
		delete m_buffer;
		m_buffer = NULL;
	}

	int scaleW = CLayer::WW()/CLayer::WS();
	int scaleH = CLayer::WH()/CLayer::WS();

	m_buffer = new unsigned char[scaleW * scaleH * BPP_RGB];

	BitmapData bmpData;
	bmpData.Height = scaleH;
	bmpData.Width  = scaleW;
	bmpData.Stride = scaleW * BPP_RGB; // ������ÿ�д�С�����з��䣬ÿ�о�û�ж����ֽ���
	bmpData.Scan0  = m_buffer;
	bmpData.PixelFormat = PixelFormat24bppRGB;
	bmpData.Reserved = NULL;
	Rect rct(0, 0, scaleW, scaleH);

	if(m_pic && m_pic->GetLastStatus()==S_OK){
		if(CLayer::WS()==1){
			m_pic->LockBits(&rct, ImageLockModeRead | ImageLockModeUserInputBuf, PixelFormat24bppRGB,  &bmpData);
			m_pic->UnlockBits(&bmpData);
		}
		else{
			Bitmap* m_scale = new Bitmap(scaleW,scaleH);
			Graphics* g = Graphics::FromImage(m_scale);
			RectF dst(0,0,scaleW,scaleH);
			g->DrawImage(m_pic, dst, 0, 0, m_pic->GetWidth(),m_pic->GetHeight(), UnitPixel);
			m_scale->LockBits(&rct, ImageLockModeRead | ImageLockModeUserInputBuf, PixelFormat24bppRGB,  &bmpData);
			m_scale->UnlockBits(&bmpData);	
			delete g;
			delete m_scale;
		}
	}
}

void CBackGround::OnDraw(HDC hdc,int x,int y){
	Clear(hdc, m_defaultbg, x, y);
	if(m_pic && m_pic->GetLastStatus()==S_OK){
		//Graphics g(hdc);
		//RectF dst(0,0,CLayer::VW(),CLayer::VH());
		//g.DrawImage(m_pic, dst, x, y, CLayer::VW()*CLayer::WS(),CLayer::VH()*CLayer::WS(), UnitPixel);

		int scaleW = CLayer::WW()/CLayer::WS();
		int scaleH = CLayer::WH()/CLayer::WS();
		CLayer::Draw(hdc, m_buffer, x/CLayer::WS(), y/CLayer::WS(), scaleW, scaleH);
	}
	else{
		CLayer::Draw(hdc, NULL, NULL, NULL, NULL, NULL);
	}
}

Bitmap& CBackGround::Getpic(){
	return *m_pic;
}

bool isBlack(unsigned char* buf, int l, int t, int r, int b, int w, int h){
	for(int i=t;i<b;i++){
		for(int j=l;j<r;j++){
			if((i*w+j+0)>=w*h)continue;
			if(buf[(i*w+j+0)*BPP_RGB]>2)return false;
			if(buf[(i*w+j+1)*BPP_RGB]>2)return false;
			if(buf[(i*w+j+2)*BPP_RGB]>2)return false;
		}
	}
	return true;
}

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

class auto_clearfolder
{
public:
    auto_clearfolder(LPCTSTR pszPath)
    {
        //���������\\����·�������û�У����к�׺�������ļ����޺�׺��Ҳ��·��
        char szPath[_MAX_PATH] = {0};
        strncpy(szPath, pszPath,_MAX_PATH);
        szPath[_MAX_PATH-1] = 0;

        
        if (szPath[strlen(szPath)-1]!='\\')
        {
			char* psp = strrchr(szPath, '\\');
			if(psp)psp[1] = 0;
        }

        PathAddBackslash(szPath);

        if (PathIsDirectory(szPath)){
			DeleteFolderAll(szPath);
			Sleep(10);
			CreateDirectory(szPath, 0);
            return;
		}

        char* psp = strchr(szPath, '\\');
        while(psp)
        {
            *psp = 0;
            if (!PathIsDirectory(szPath))
            {
                if (!CreateDirectory(szPath, 0))
                    return;
            }
            *psp = '\\';
            psp = strchr(psp + 1, '\\');
        }
    };

	bool DeleteFolderAll(const char* pSrcPath )
	{
		if(pSrcPath == NULL)
			return false;

		char pwcPath[MAX_PATH];
		strcpy(pwcPath , pSrcPath);
		int ilen = strlen(pwcPath);

		if (pwcPath[ilen-1] == L'\\')
		{
			pwcPath[ilen-1] = 0;
		}


		char wcPath[MAX_PATH] = {0};
		strcpy(wcPath,pwcPath);
		strcat(wcPath,_T("\\*.*"));
		WIN32_FIND_DATA FindFileData;
		ZeroMemory(&FindFileData,sizeof(WIN32_FIND_DATA));

		HANDLE hFindFile = FindFirstFile(wcPath,&FindFileData);

		if(hFindFile == INVALID_HANDLE_VALUE)
			return false;

		BOOL bContinue = true;

		while (bContinue != false)
		{
			//bIsDotsΪ���ʾ��.��..
			bool bIsDots = (strcmp(FindFileData.cFileName,_T(".")) == 0 || strcmp(FindFileData.cFileName,_T("..")) == 0);
			strcpy(wcPath,pwcPath);
			strcat(wcPath,_T("\\"));
			strcat(wcPath,FindFileData.cFileName);
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 && bIsDots == false)
			{
				//��Ŀ¼,���ٽ����Ŀ¼

				DeleteFolderAll(wcPath);
				//Ѱ����һ�ļ�
				bContinue = FindNextFile(hFindFile,&FindFileData);
				continue;
			}

			if (bIsDots == false)
			{
				//���ļ�ɾ��֮
				DeleteFile(wcPath);
			}
			//Ѱ����һ�ļ�
			bContinue = FindNextFile(hFindFile,&FindFileData);

		}

		FindClose(hFindFile);

		//ɾ����Ŀ¼
		RemoveDirectory(pwcPath);
		return true;
	}
};

void CBackGround::Save(string outdir){
	if(m_pic==NULL)return;
	int mRealWidth =  m_pic->GetWidth();
	int mRealHeight =  m_pic->GetHeight();
	Bitmap* bmpCombine =new Bitmap(512,512);
	
	auto_clearfolder cd(outdir.c_str());
	Graphics  g(bmpCombine);
	int hc = CLayer::WH()/512+1;
	int wc = CLayer::WW()/512+1;
	for(int i=0;i<=hc;i++){
		for(int j=0;j<=wc;j++){
			//if(isBlack(m_buffer,j*512,i*512,(j+1)*512,(i+1)*512,mRealWidth,mRealHeight))continue;
			g.Clear(Color(0x00));
			char path[256]={0};
			RectF dst(0.0,0.0,512.0,512.0);
			g.DrawImage(m_pic, dst, j*512, i*512, 512, 512, UnitPixel);
			sprintf(path,"%s_r%d_c%d.jpg",outdir.c_str(),i+1,j+1);
			SavePicture(*bmpCombine,"image/jpeg",path);
		}
	}
	delete bmpCombine;
}