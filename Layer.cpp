#include "stdafx.h"
#include "Layer.h"

int CLayer::m_Vwidth=1;
int CLayer::m_Vheight=1;
int CLayer::m_Cwidth=1;
int CLayer::m_Cheight=1;
int CLayer::m_Wwidth=1;
int CLayer::m_Wheight=1;

int CLayer::m_ScaleXY=1;


bool CLayer::m_bErase=0;
MAP_STATE  CLayer::m_GridType=MAP_NORMAL;
CLayer::CLayer(UINT width,UINT height)
	:m_width(width)
	,m_height(height)
{ 
    HDC hdc;  
    //获取屏幕显示DC          
    hdc = GetDC(NULL);  
    //创建内存DC  
	m_hdc = CreateCompatibleDC(hdc);  
      
	BITMAPINFO bmpinfo={0};  
    bmpinfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);  
    bmpinfo.bmiHeader.biWidth=width;  
    bmpinfo.bmiHeader.biHeight=(0-height);  
    bmpinfo.bmiHeader.biPlanes=1;  
    bmpinfo.bmiHeader.biBitCount=24;  
    bmpinfo.bmiHeader.biCompression=BI_RGB;  
    bmpinfo.bmiHeader.biSizeImage=width*height*3;  
    bmpinfo.bmiHeader.biXPelsPerMeter=0;  
    bmpinfo.bmiHeader.biClrImportant=0;  
    bmpinfo.bmiHeader.biClrUsed=0;  
  
	m_bmp=CreateDIBSection(m_hdc,&bmpinfo,DIB_RGB_COLORS,(void**) &m_buf,NULL,0);  

    HGDIOBJ hOldSel = SelectObject(m_hdc,m_bmp);  
	DeleteObject(hOldSel);

    ReleaseDC(NULL, hdc);
} 


CLayer::~CLayer(void)
{
	ReleaseDC(NULL, m_hdc);
}

void CLayer::Draw(HDC dc, unsigned char* buf, int x, int y, int w, int h){
	if(buf){
		int top = 0;
		int left = 0;
		int rw = m_width;
		int rh = m_height;

		if( y < 0 ){
			top = -y;
		}

		if( x < 0 ){
			left = -x;
			x = 0;
		}
	
		if( h - y < rh - top ){
			rh = h - y;
		}

		if( w - x < rw - left ){
			rw = w - x;
		}

		if(rw - left>0){
			for(int i=top;i<rh;i++){
				memcpy(
					m_buf+( i*m_width + left )*BPP_RGB,
					buf + ((y+i)*w + x )*BPP_RGB,
					(rw - left)*BPP_RGB);
			}
		}
	}
	BitBlt(dc,0,0,m_width,m_height,m_hdc,0,0,SRCCOPY);
}

void CLayer::Clear(HDC dc, Image* img,int x, int y){
	memset(m_buf,0x0,m_width*m_height*BPP_RGB);
	Graphics g(m_hdc);
	int offsetX=0;
	int offsetY=0;

	offsetX = abs(x)%img->GetWidth();
	offsetY = abs(y)%img->GetHeight();
	if(x<0){
		offsetX=img->GetWidth()-offsetX;
	}

	if(y<0){
		offsetY=img->GetHeight()-offsetY;
	}

	g.TranslateTransform((float)(-offsetX), (float)(-offsetY));
	TextureBrush headbrush(img, WrapModeTile );
	g.FillRectangle(&headbrush, RectF(0.f, 0.f, (float)width()*2,(float)height()*2));
}

Gdiplus::Pen* CLayer::GetPen(ARGB color,float size){
	map<ARGB,Gdiplus::Pen*>::iterator iter = Pen.find(color);
	if(iter==Pen.end()){
		Pen.insert(std::make_pair(color, new Gdiplus::Pen(color, size)));
		iter = Pen.find(color);
	}
	return iter->second;
}


Gdiplus::SolidBrush* CLayer::GetBrush(ARGB color){
	map<ARGB,Gdiplus::SolidBrush*>::iterator iter = Brush.find(color);
	if(iter==Brush.end()){
		Brush.insert(std::make_pair(color,new Gdiplus::SolidBrush(color)));
		iter = Brush.find(color);
	}
	return iter->second;
}



void CLayer::SavePicture(Bitmap& bmpCombine,const char *format, const char *path)
{
	USES_CONVERSION;
	int nRet = -1;
	ImageCodecInfo * pCodecInfo = NULL;
	UINT nNum = 0,nSize = 0;
	GetImageEncodersSize(&nNum,&nSize);
	if (nSize<0)
	{
		return;
	}
	pCodecInfo = new ImageCodecInfo[nSize];
	if (pCodecInfo==NULL)
	{
		return;
	}
	GetImageEncoders(nNum,nSize,pCodecInfo);
	for (UINT i=0; i<nNum; i++)
	{
		if (wcscmp(pCodecInfo[i].MimeType,A2W(format))==0)
		{
			bmpCombine.Save(A2W(path), &pCodecInfo[i].Clsid);
			break;
		}
	}
	delete[] pCodecInfo;
}

void CLayer::CombinePic(Bitmap& bmpCombine, string strPic2,int x,int y)
{
	USES_CONVERSION;
	Bitmap bmp(A2W(strPic2.c_str()));

	if(bmp.GetLastStatus()==S_OK){
		Graphics * pG = NULL;
		pG = Graphics::FromImage(&bmpCombine);
		if (pG!=NULL)
		{
			pG->DrawImage(&bmp,x,y,0,0,512,512,UnitPixel);
		}
	}
}
