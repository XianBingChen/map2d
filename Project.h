#pragma once
typedef void (*SetTimerPr)(int,int);

class CProject
{
private:
	SetTimerPr SetTimer;
	int m_autox;
	int m_autoy;
	int m_ox;
	int m_oy;
	int m_sx;
	int m_sy;
	int m_width;
	int m_height;
	int m_backOnly;
	float ScaleX;
	float ScaleY;
	int m_bDragMinimap;

	bool m_bHover;
	

	HDC m_cache;
	CMiniMap m_minimap;
	CDrawGrid m_grid;
	CBackGround m_background;

public:
	CProject(SetTimerPr _SetTimer,HWND hWnd,int width,int height);
	~CProject(void);
	void Open();
	void OnTimer(int);
	void OnFlresh(HDC);
	void OnSize(int x,int y);
	void OnDrag(int x,int y,int ox,int oy);
	void OnMouseUp(int x,int y);
	void OnMouseMove(int x,int y);
	void OnMouseDown(int x,int y, bool bShift);

	void Home(int,int);
	void Left(bool set = true);
	void Right(bool set = true);
	void Up(bool set = true);
	void Down(bool set = true);

	void Clear();
	void StarDrag(int ox,int oy);
	void Block(int ox,int oy);

	void Load();
	void Save();
	void DoErase(bool b);
	void SelectLayer(MAP_STATE type);
	void CbRes();
	void SetPostion(int x,int y);
	void SetOffset(int ox,int oy);
	void SetScaleXY(int s);
	void GetScaleXY(int&x,int &y);
	inline void ShowBack(bool b){
		m_backOnly=b;
	}
};
