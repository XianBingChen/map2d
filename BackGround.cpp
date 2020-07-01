#include "stdafx.h"
#include "resource.h"
#include "BackGround.h"

CBackGround::CBackGround(int width, int height)
	:CLayer(width,height)
	,m_pic(NULL)
{
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
	if(m_buffer){
		delete m_buffer;
		m_buffer = NULL;
	}
	
	try{
		m_pic = Bitmap::FromFile(A2W(m_url.c_str()));
		if(m_pic->GetLastStatus()==S_OK){
			DWORD Width, Height;
			Width = m_pic->GetWidth();
			Height = m_pic->GetHeight();
			CLayer::setWWH(Width, Height);
			m_buffer = new unsigned char[Width * Height * BPP_RGB];

			BitmapData bmpData;
			bmpData.Width  = Width;
			bmpData.Height = Height;
			bmpData.Stride = Width * BPP_RGB; // 缓冲区每行大小，自行分配，每行就没有多余字节了
			bmpData.Scan0  = m_buffer;
			bmpData.PixelFormat = PixelFormat24bppRGB;
			bmpData.Reserved = NULL;
			Rect rct(0, 0, Width, Height);
			m_pic->LockBits(&rct, ImageLockModeRead | ImageLockModeUserInputBuf, PixelFormat24bppRGB,  &bmpData);
			m_pic->UnlockBits(&bmpData);
		}
		else{
			MessageBox(NULL,"图片加载失败","资源加载",NULL);
		}
	}
	catch(...){
		m_buffer = NULL;
		char msg[1024];
		sprintf_s(msg,1024,"您系统内存太小，无法加载资源文件");
		MessageBox(NULL,msg,"内存不足",NULL);
	}
}

void CBackGround::OnDraw(HDC hdc,int x,int y){
	Clear(hdc, m_defaultbg, x, y);
	if(m_pic && m_pic->GetLastStatus()==S_OK){
		CLayer::Draw(hdc, m_buffer, x, y, m_pic->GetWidth(), m_pic->GetHeight());
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

class auto_folder
{
public:
    auto_folder(LPCTSTR pszPath)
    {
        //如果后面有\\则是路径，如果没有，但有后缀，则是文件，无后缀则也是路径
        char szPath[_MAX_PATH] = {0};
        strncpy(szPath, pszPath,_MAX_PATH);
        szPath[_MAX_PATH-1] = 0;

        
        if (szPath[strlen(szPath)-1]!='\\')
        {
			char* psp = strrchr(szPath, '\\');
			if(psp)psp[1] = 0;
        }

        PathAddBackslash(szPath);

        if (PathIsDirectory(szPath))
            return;

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
};

void CBackGround::Save(string outdir){
	if(m_pic==NULL)return;
	int mRealWidth =  m_pic->GetWidth();
	int mRealHeight =  m_pic->GetHeight();
	Bitmap* bmpCombine =new Bitmap(512,512);
	
	auto_folder cd(outdir.c_str());
	Graphics * pG = NULL;
	pG = Graphics::FromImage(bmpCombine);
	if (pG!=NULL)
	{	
		int hc = CLayer::WH()/512+1;
		int wc = CLayer::WW()/512+1;
		for(int i=0;i<=hc;i++){
			for(int j=0;j<=wc;j++){
				//if(isBlack(m_buffer,j*512,i*512,(j+1)*512,(i+1)*512,mRealWidth,mRealHeight))continue;
				pG->Clear(Color(0x00));
				char path[256]={0};
				RectF dst(0.0,0.0,512.0,512.0);
				pG->DrawImage(m_pic, dst, j*512, i*512, 512, 512, UnitPixel);
				sprintf(path,"%s_r%d_c%d.jpg",outdir.c_str(),i+1,j+1);

				SavePicture(*bmpCombine,"image/jpeg",path);
			}
		}
		delete pG;
	}
	delete bmpCombine;
}