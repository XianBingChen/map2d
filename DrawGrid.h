#pragma once

class CDrawGrid:public CLayer,public CMapinfo
{
private:
	bool doMask;
	int cursorX;
	int cursorY;
	int hoverX;
	int hoverY;

	list<opstatus> m_onopt;
	list<list<opstatus>> m_opUndo;
	list<list<opstatus>> m_opRedo;
public:
	CDrawGrid(int width, int height);
	~CDrawGrid(void);
	void OnDraw(HDC dc,int x,int y);
	void OnDrawMini(Graphics& g,float x,float y,float sx,float sy);
	void Select(int x,int y);
	void Hover(int x,int y);
	void SetMasks(int x,int y,MAP_STATE,bool);
	void SetMaskOne(int x2,int y2,MAP_STATE a,bool set);
	inline void GetPosition(int &x,int &y){
		x = cursorX;
		y = cursorY;
	}
	inline bool IsMaskNow(){return doMask;};
	void onMouseUp();
	void DoEdit(bool Undo);
};

