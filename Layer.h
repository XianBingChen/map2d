#pragma once

#define BPP_RGB 3 

enum MAP_STATE
{
	MAP_NORMAL=0,
	MAP_BLOCK=1,
	MAP_MARK=2,
	MAP_WATER=3,
};

class CLayer
{
private:
	static int m_Cwidth;
	static int m_Cheight;

	static int m_Vwidth;
	static int m_Vheight;

	static int m_Wwidth;
	static int m_Wheight;

	static bool m_bErase;
	
	static MAP_STATE m_GridType; 


	int m_width;
	int m_height;

	HDC m_hdc;
	HBITMAP m_bmp;
	unsigned char* m_buf;
	map<ARGB,Gdiplus::Pen*> Pen;
	map<ARGB,Gdiplus::SolidBrush*> Brush;

public:
	CLayer(UINT width,UINT height);
	~CLayer(void);
	Gdiplus::Pen* GetPen(ARGB,float size=1.0f);
	Gdiplus::SolidBrush* GetBrush(ARGB);
	inline int width(){ return m_width;}
	inline int height(){ return m_height;}

	inline static int VW(){ return m_Vwidth;}
	inline static int VH(){ return m_Vheight;}
	inline static void setVWH(int w,int h){ m_Vwidth = w; m_Vheight = h;}

	inline static int CW(){ return m_Cwidth;}
	inline static int CH(){ return m_Cheight;}
	inline static void setCWH(int w,int h){ m_Cwidth = w;m_Cheight = h;}

	inline static int WW(){ return m_Wwidth;}
	inline static int WH(){ return m_Wheight;}

	inline static void setWWH(int w,int h){
		m_Wwidth = w ;
		m_Wheight = h ;
	}

	static void SavePicture(Bitmap& bmpCombine,const char *format, const char *path);
	static void CombinePic(Bitmap& bmpCombine, string strPic2,int x,int y);

	inline static bool isErase(){
		return m_bErase;
	}

	inline static void setErase(bool b){
		m_bErase = b;
	}


	inline static MAP_STATE GetType(){
		return m_GridType;
	}

	inline static void SetType(MAP_STATE b){
		m_GridType = b;
	}

	inline HDC GetDc(){return m_hdc;}
	inline HBITMAP GetHBITMAP(){return m_bmp;}
	void Clear(HDC dc, Image* img,int x, int y);
	void Draw(HDC dc, unsigned char* buf, int x, int y, int w, int h);
	virtual void OnDraw(HDC dc,int x,int y)=0;
};

