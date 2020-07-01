#pragma once

class CBackGround:public CLayer
{
private:
	string m_url;
	Bitmap* m_pic;
	Image* m_defaultbg;
	unsigned char *m_buffer;
public:
	CBackGround(int width, int height);
	~CBackGround(void);
	void FreeRes();
	void ReadRes();
	void SetRes( string url );
	void Save(string outdir);
	void OnDraw(HDC dc,int x,int y);
	Bitmap& Getpic();
};

