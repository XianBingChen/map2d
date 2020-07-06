#include "stdafx.h"
#include "DrawGrid.h"


CDrawGrid::CDrawGrid(int width, int height)
	:CLayer(width,height)
{
}

CDrawGrid::~CDrawGrid(void)
{
}

void CDrawGrid::OnDrawMini(Graphics& graphics,float x,float y,float sx,float sy){
	USES_CONVERSION;

	int i,j;
	map<int,map<int,int>>::iterator iter = m_cells.begin();
	for(;iter!=m_cells.end();iter++)
	{
		i = iter->first;
		map<int,int> row = iter->second;
		map<int,int>::iterator iterx = row.begin();
		for(;iterx!=row.end();iterx++)
		{
			j = iterx->first;
			int& mask = iterx->second;
			
			//if((CLayer::GetType()==MAP_BLOCK || !CLayer::GetType()) &&  mask & (1<<MAP_BLOCK))
			if(mask & (1<<MAP_BLOCK)){
				graphics.FillRectangle(GetBrush(Color::MakeARGB(0x66,0xFF,0x00,0x0)), x+(j*CLayer::CW())*sx, y+(i*CLayer::CH())*sy, CLayer::CW()*sx, CLayer::CH()*sy);
			}
					
			//if((CLayer::GetType()==MAP_MARK || !CLayer::GetType()) &&  mask & (1<<MAP_MARK))
			if( mask & (1<<MAP_MARK)){
				graphics.FillRectangle(GetBrush(Color::MakeARGB(0x66,0x00,0xFF,0x00)), x+(j*CLayer::CW())*sx, y+(i*CLayer::CH())*sy, CLayer::CW()*sx, CLayer::CH()*sy);
			}
		}
	}

	Gdiplus::FontFamily fontFamily(L"ºÚÌå");
	Gdiplus::Font font(&fontFamily, 16, Gdiplus::FontStyleRegular, Gdiplus::Unit::UnitPixel);

	bstr_t text = A2W(m_filename.c_str());
	if(wcslen(text)>0){
		switch (CLayer::GetType())
		{
		case MAP_BLOCK:
			text+="_";
			text+="ÕÏ°­²ã";
		break;
		case MAP_MARK:
			text+="_";
			text+="ÕÚµ²²ã";
		break;
		default:
			text+="_";
			text+="±³¾°";
			break;
		}

		if(CLayer::GetType()!=0){
			text+="[";
			text+=CLayer::isErase()?"ÏðÆ¤":"Ç¦±Ê";
			text+="]";
		}
	}

	graphics.DrawString(text, (INT)wcslen(text), &font,Gdiplus::PointF(x, y),GetBrush(Color::MakeARGB(0xFF,0xFF,0xFF,0xFF)));
}

void CDrawGrid::OnDraw(HDC dc,int x,int y){
	Graphics graphics(dc);


	int left = (x/CLayer::CW());
	int top = (y/CLayer::CH());

	int width = CLayer::width()+1;
	int height= CLayer::height()+1;

	float cw = (float)CLayer::CW()/CLayer::WS();
	float ch = (float)CLayer::CH()/CLayer::WS();

	float sx = (float)(-x%CLayer::CW())/CLayer::WS();
	float sy = (float)(-y%CLayer::CH())/CLayer::WS();


	if(CLayer::GetType()){
		for(int i=0;i<=height/ch+1;i++)
		{
			map<int,map<int,int>>::iterator iter = m_cells.find(i+top);
			if(iter!=m_cells.end()){
				for(int j=0;j<=width/cw+1;j++)
				{
					map<int,int>::iterator iterx = iter->second.find(j+left);

					if(iterx!=iter->second.end()){
						int& mask = iterx->second;
					
						//if((CLayer::GetType()==MAP_BLOCK || !CLayer::GetType()) &&  mask & (1<<MAP_BLOCK))
						if(mask & (1<<MAP_BLOCK)){
							graphics.FillRectangle(GetBrush(Color::MakeARGB(0x66,0xFF,0x00,0x00)), sx+(j*cw), sy+(i*ch), cw, ch);
						}
					
						//if((CLayer::GetType()==MAP_MARK || !CLayer::GetType()) &&  mask & (1<<MAP_MARK))
						if(mask & (1<<MAP_MARK)){
							graphics.FillRectangle(GetBrush(Color::MakeARGB(0x66,0x00,0xFF,0x00)), sx+(j*cw), sy+(i*ch), cw, ch);
						}
					}
				}
			}
		}
	}

	//Í¨¹ýÑ­»·»­³öºáÏß
	float offset = 0.0f;
	for(int i=0;i<=height;i++)
	{
		graphics.DrawLine(GetPen(Color::Black), 0.0f, sy+offset, (float)CLayer::width(), sy+offset);
		offset+=ch;
	}

	//Í¨¹ýÑ­»·»­³öÊúÏß
	offset = 0.0f;
	for(int i=0;i<=width;i++)
	{
		graphics.DrawLine(GetPen(Color::Black), sx+offset, 0.0f, sx+offset, (float)CLayer::height());
		offset+=cw;
	}
	

	for(int i=0;i<CLayer::GetPenSize();i++){
		for(int j=0;j<CLayer::GetPenSize();j++){
			graphics.FillRectangle(GetBrush(Color::MakeARGB(0x66,0x4e,0x4e,0xf2)),sx+(((hoverX-CLayer::GetPenSize()/2)-left+i)*cw),sy+(((hoverY-CLayer::GetPenSize()/2)-top+j)*ch),cw,ch);
		}
	}
	
	graphics.DrawRectangle(
		GetPen(Color::MakeARGB(0xFF,0x4e,0x4e,0xf2)),
		sx+(((hoverX-CLayer::GetPenSize()/2)-left)*cw),
		sy+(((hoverY-CLayer::GetPenSize()/2)-top)*ch),
		(float)cw*CLayer::GetPenSize(),
		(float)ch*CLayer::GetPenSize()
	);

	graphics.DrawRectangle(GetPen(Color::MakeARGB(0xFF,0x4e,0x4e,0xf2)),sx+((cursorX-left)*cw)+1.0f,sy+((cursorY-top)*ch)+1.0f,cw-2.0f,ch-2.0f);

}

void CDrawGrid::Select(int x,int y){
	cursorX = x/CLayer::CW();
	cursorY = y/CLayer::CH();
}

void CDrawGrid::Hover(int x,int y){
	hoverX = x/CLayer::CW();
	hoverY = y/CLayer::CH();
}


void CDrawGrid::SetMaskOne(int x2,int y2,MAP_STATE a,bool set){
	if(x2<0 || y2<0)return;
	x2 /= CLayer::CW();
	y2 /= CLayer::CH();
	x2 -= CLayer::GetPenSize()/2;
	y2 -= CLayer::GetPenSize()/2;

	for(int i=0;i<CLayer::GetPenSize();i++){
		for(int j=0;j<CLayer::GetPenSize();j++){
			opstatus o ={x2+i,y2+j,0};
			if(SetMask(o,a,set)){
				m_onopt.push_front(o);
			}
			else{
				OutputDebugPrintf("test!");
			}
		}
	}
}

void CDrawGrid::SetMasks(int x2,int y2,MAP_STATE a,bool set){
	if(x2<0 || y2<0)return;
	x2/=CLayer::CW();
	y2/=CLayer::CH();
	

	int x1 = cursorX-CLayer::GetPenSize()/2;
	int y1 = cursorY-CLayer::GetPenSize()/2;

	x2 += CLayer::GetPenSize()/2;
	y2 += CLayer::GetPenSize()/2;

	if(cursorX>x2){
		x1 = x2;
		x2 = cursorX;
	}
	if(cursorY>y2){
		y1 = y2;
		y2 = cursorY;
	}
	
	for(int i = x1;i<=x2;i++){
		for(int j = y1;j<=y2;j++){
			opstatus o ={i,j,a};
			SetMask(o,a,set);
			m_onopt.push_front(o);
		}
	}
}

void CDrawGrid::onMouseUp(){
	m_opUndo.push_front(m_onopt);
	if(m_opUndo.size()>20)m_opUndo.resize(20);
	m_opRedo.clear();
	m_onopt.clear();
}

void CDrawGrid::DoEdit(bool Undo){
	if(Undo){
		if(m_opUndo.size()>0){
			list<opstatus> iter = m_opUndo.front();
			list<opstatus>::iterator child = iter.begin();
			if(iter.size()>=2){
				OutputDebugPrintf("test!");
			}
			while(child!=iter.end()){
				opstatus o = *child;
				ReSetMask(o);
				*child = o;
				++child;
			}
			m_opRedo.push_front(iter);	
			m_opUndo.pop_front();
		}
	}
	else{
		if(m_opRedo.size()>0){
			list<opstatus> iter = m_opRedo.front();
			list<opstatus>::iterator child = iter.begin();
			if(iter.size()>=2){
				OutputDebugPrintf("test!");
			}
			while(child!=iter.end()){
				opstatus o = *child;
				ReSetMask(o);
				*child = o;
				++child;
			}
			m_opUndo.push_front(iter);	
			m_opRedo.pop_front();
		}
	}
}