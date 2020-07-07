#pragma once
class CDrawGrid;
class CMiniMap:public CLayer
{
private:
	float m_scaleX;
	float m_scaleY;
	CDrawGrid* m_grid;
public:
	CMiniMap(int width, int height);
	~CMiniMap(void);
	void OnDraw(HDC dc,int x,int y);
	void Create(Bitmap& m_pic);
	bool inrect(int x,int y);
	float px();
	float py();
	float mw();
	float mh();
	void scaleXY(int& x,int& y);
	void Hover(int& x,int& y);
	inline void SetGrid(CDrawGrid* grid){ m_grid = grid;};
};

