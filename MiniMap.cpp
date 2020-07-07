#include "stdafx.h"
#include "MiniMap.h"


CMiniMap::CMiniMap(int width, int height):CLayer(240, 160)
{
	m_grid = NULL;
	m_scaleX = mw()/CLayer::WW();
	m_scaleY = mh()/CLayer::WH();
}


CMiniMap::~CMiniMap(void)
{
}

float CMiniMap::px(){
	return VW() - mw() - 10;
}

float CMiniMap::py(){
	return 10;
}

float CMiniMap::mw(){
	return 240.f;
}

float CMiniMap::mh(){
	return 160.f;
}

void CMiniMap::OnDraw(HDC dc,int x,int y){
	float rx = (float)x;
	float ry = (float)y;

	float cw = 10.f;
	float ch = cw * CH()/ CW();

	float ox = (rx+VW()/2);
	float oy = (ry+VH()/2);

	Graphics g(dc);

	Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromHBITMAP(GetHBITMAP(),0);
	g.DrawImage(bmp, px(), py(), mw(), mh());
	delete bmp;

	if(m_grid){
		m_grid->OnDrawMini(g,px(), py(),m_scaleX,m_scaleY);
	}

	g.DrawRectangle(GetPen(Color::MakeARGB(0xFF,0x08,0xfe,0x80),2.f), px()-1.f, py()-1.f, mw()+1.f,  mh()+1.f);
	g.DrawRectangle(GetPen(Color::MakeARGB(0xFF,0xFF,0x00,0x00),1.f), px() + ox*m_scaleX+1-cw/2, py() + oy*m_scaleY+1-ch/2, cw, ch);

}

void CMiniMap::Create(Bitmap& m_pic){
	Graphics g(GetDc());
	g.DrawImage(&m_pic, 0.f, 0.f, mw(), mh());
	m_scaleX = mw()/CLayer::WW();
	m_scaleY = mh()/CLayer::WH();
}

bool CMiniMap::inrect(int x,int y){
	return (x>px() && x<px()+mw() && y>py() && y<py()+mh());
}

void CMiniMap::scaleXY(int& x,int& y){
	x = float(x) /m_scaleX;
	y = float(y) /m_scaleY;
}

void CMiniMap::Hover(int& x,int& y){
	float rx = (float)x;
	float ry = (float)y;

	rx -= px();
	ry -= py();
	rx /= m_scaleX;
	ry /= m_scaleY;
	x = rx;
	y = ry;
}