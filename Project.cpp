#include "stdafx.h"
#include "Project.h"

enum TIMERTYPE{
	LoadBackGround=1
};

CProject::CProject(SetTimerPr _SetTimer,HWND hWnd,int width,int height)
	:SetTimer(_SetTimer)
	,m_width(width)
	,m_height(height)
	,m_grid(width,height)
	,m_background(width,height)
	,m_minimap(width,height)
{
	m_ox=0;
	m_oy=0;
	m_sx=0;
	m_sy=0;
	m_autox = 0;
	m_autoy = 0;
	m_backOnly = true;
	m_bHover = false;
	m_bDragMinimap = false;
    HDC hdc = GetDC(hWnd);
    m_cache = CreateCompatibleDC(hdc);

    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    PVOID m_pBits;
    HBITMAP  m_hBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&m_pBits, NULL, NULL);
    SelectObject(m_cache, m_hBmp);
	ReleaseDC(hWnd,hdc);
	m_minimap.SetGrid(&m_grid);

	CLayer::setCWH(66,44);
}


CProject::~CProject(void)
{
}

void CProject::OnTimer(int id){
	switch(id){
	case LoadBackGround:
		m_background.ReadRes();
		m_minimap.Create(m_background.Getpic());
		SetPostion((CLayer::WW()-m_width)/2,(CLayer::WH()-m_height)/2);
		break;
	default:
		break;
	}
}

void CProject::OnFlresh(HDC dc){
	if(m_autox!=0){
		SetOffset(m_ox+m_autox*CLayer::CW()*CLayer::WS(),m_oy);
	}
	if(m_autoy!=0){
		SetOffset(m_ox,m_oy+m_autoy*CLayer::CH()*CLayer::WS());
	}

	int x = m_sx-m_ox;
	int y = m_sy-m_oy;

	int sx,sy;
	GetScaleXY(sx,sy);

	RECT rc = {0, 0, CLayer::VW(), CLayer::VH()};
	HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	::FillRect(m_cache,&rc,hBrush);
	m_background.OnDraw(m_cache, sx, sy);
	m_grid.OnDraw(m_cache, sx, sy);
	m_minimap.OnDraw(m_cache, x, y);
	SetStretchBltMode(dc,HALFTONE);
	BitBlt(dc,0, 0, CLayer::VW(), CLayer::VH(), m_cache, 0, 0,SRCCOPY);
}

void CProject::OnSize(int w,int h){
	CLayer::setVWH(w,h);
}

void CProject::StarDrag(int x,int y){
	if(m_minimap.inrect(x,y)){
		m_bDragMinimap = true;
	}
	OnDrag(x,y,0,0);
}

void CProject::OnDrag(int x,int y,int ox,int oy){
	if(m_bDragMinimap){
		m_minimap.Hover(x,y);
		SetPostion(x-CLayer::VW()/2,y-CLayer::VH()/2);
	}
	else{
		SetOffset(CLayer::WS()*ox,CLayer::WS()*oy);
	}
}

void CProject::OnMouseMove(int x,int y){
	if(m_bHover){
		int sx,sy;
		GetScaleXY(sx,sy);
		sx += x*CLayer::WS();
		sy += y*CLayer::WS();
		if(sx>=0 && sy>=0){
			if(!m_backOnly){
				m_grid.SetMaskOne(sx, sy, m_grid.GetType(), !CLayer::isErase());
			}
			m_grid.Hover(sx, sy);
		}
	}
}

void CProject::OnMouseDown(int x,int y, bool bShift){
	if(m_minimap.inrect(x,y)){
		return;
	}
	m_bHover = true;
	int sx,sy;
	GetScaleXY(sx,sy);
	sx += x*CLayer::WS();
	sy += y*CLayer::WS();
	if(sx>=0 && sy>=0){
		if(bShift)
			m_grid.SetMasks(sx, sy, m_grid.GetType(), !CLayer::isErase());
		else{
			m_grid.SetMaskOne(sx, sy, m_grid.GetType(), !CLayer::isErase());
			m_grid.Hover(sx, sy);
		}
	}
}

void CProject::OnMouseUp(int ox,int oy){
	m_bHover = false;
	m_bDragMinimap = false;
}

void CProject::Home(int w,int h){
	int x,y;
	m_ox = 0;
	m_oy = 0;
	m_grid.GetPosition(x,y);
	SetPostion((x*CLayer::CW())-(w-CLayer::CW())/2,(y*CLayer::CH())-(h-CLayer::CH())/2);
}

void CProject::Left(bool set){
	m_autox =set?1:0;
}

void CProject::Right(bool set){
	m_autox =set?-1:0;
}

void CProject::Up(bool set){
	m_autoy = set?1:0;
}

void CProject::Down(bool set){
	m_autoy =set?-1:0;
}

void CProject::Clear(){
	m_sx -= m_ox;
	m_sy -= m_oy;
	m_ox = 0;
	m_oy = 0;
	m_autox = 0;
	m_autoy = 0;
	m_bDragMinimap = false;
}

void CProject::DoErase(bool b){
	CLayer::setErase(b);
}

void CProject::SelectLayer(MAP_STATE type){
	m_grid.SetType(type);
}

void CProject::SetPostion(int x,int y){
	if(x<m_ox || CLayer::VW()> CLayer::WW()){
		m_sx = m_ox;
	}
	else
	if(x>(m_ox + CLayer::WW()-CLayer::VW())){
		m_sx = m_ox + CLayer::WW()-CLayer::VW();
	}
	else{
		m_sx=x;
	}

	if(y<m_oy || CLayer::VH()>CLayer::WH()){
		m_sy = m_oy;
	}
	else 
	if(y>(m_oy + CLayer::WH()-CLayer::VH())){
		m_sy = m_oy + CLayer::WH()-CLayer::VH();
	}
	else{
		m_sy=y;
	}
}

void CProject::SetOffset(int ox,int oy){
	if(m_sx-ox<0 || CLayer::VW()> CLayer::WW()){
		m_ox = m_sx;
	}
	else
	if(m_sx-ox>(CLayer::WW()-CLayer::VW())){
		m_ox = m_sx - (CLayer::WW()-CLayer::VW());
	}
	else{
		m_ox=ox;
	}

	if(m_sy-oy<0 || CLayer::VH()>CLayer::WH()){
		m_oy = m_sy;
	}
	else if(m_sy-oy>(CLayer::WH()-CLayer::VH())){
		m_oy = m_sy - (CLayer::WH()-CLayer::VH());
	}
	else{
		m_oy=oy;
	}
}

void CProject::SetPenSize(int s){
	CLayer::SetPenSize(s);
}

void CProject::SetScaleXY(int s){
	CLayer::setWS(s);
	m_background.ScaleXY();
}

void CProject::GetScaleXY(int&x,int &y){
	float sx2 = (float)(m_sx - m_ox);
	float sy2 = (float)(m_sy - m_oy);
	float ox = CLayer::VW()*(CLayer::WS()-1)/2;
	float oy = CLayer::VH()*(CLayer::WS()-1)/2;
	x = sx2-ox;
	y = sy2-oy;
}

void CProject::Open(){
	CMapinfo& m_map = m_grid;
	TCHAR szFileName[MAX_PATH] = {};
	OPENFILENAME openFileName = {};
	openFileName.lStructSize = sizeof(OPENFILENAMEW);
	openFileName.nMaxFile = MAX_PATH;//这个必须设置，不设置的话不会出现打开文件对话框  
	openFileName.lpstrFilter = "文本文件（*.jpg*）\0*.jpg\0任何文件（*.*）\0*.*\0";
	openFileName.lpstrFile = szFileName;
	openFileName.nFilterIndex = 1;
	openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	char szCurDir[MAX_PATH];
    GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	char* fd = strrchr(szCurDir,'\\');
	fd[0]='\0';
	strcat(szCurDir,"\\source\\*.jpg");

	openFileName.lpstrFile = szCurDir;

	string path = m_map.GetResPath();
	if(path.empty()){
		MessageBox(NULL,"新地图请先保存！","未知地图",NULL);
		return;
	}
	if (GetOpenFileName(&openFileName))
	{
		m_background.FreeRes();		

		char dir[256];
		GetCurrentDirectory(256,dir);
		strcat(dir,"\\");
		strcat(dir,m_map.GetResPath().c_str());

		::CopyFile(openFileName.lpstrFile,dir,false);

		m_background.SetRes(m_map.GetResPath());
		SetTimer(LoadBackGround,10);
	}
}

void CProject::Load(){
	TCHAR szFileName[MAX_PATH] = {};
	OPENFILENAME openFileName = {};
	openFileName.lStructSize = sizeof(OPENFILENAMEW);
	openFileName.nMaxFile = MAX_PATH;//这个必须设置，不设置的话不会出现打开文件对话框  
	openFileName.lpstrFilter = "文本文件（*.mapo）\0*.mapo\0任何文件（*.*）\0*.*\0";
	openFileName.lpstrFile = szFileName;
	openFileName.nFilterIndex = 1;
	openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	char szCurDir[MAX_PATH];
    GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	char* fd = strrchr(szCurDir,'\\');
	fd[0]='\0';
	strcat(szCurDir,"\\Client\\runtime\\res\\map\\*.mapo");

	openFileName.lpstrFile = szCurDir;

	if (GetOpenFileName(&openFileName))
	{
		CMapinfo& m_map = m_grid;
		m_map.Clear();

		if(!m_map.FromFile(openFileName.lpstrFile)){
			MessageBox(NULL, "打开地图文件失败！", NULL, NULL);
			return;
		}

		m_background.SetRes(m_map.GetResPath());
		SetTimer(LoadBackGround,10);
	}
}

void CProject::Save(){
	CMapinfo& m_map = m_grid;

	string savepath = m_map.GetSavePath();

	TCHAR szFileName[MAX_PATH] = {};
	OPENFILENAME openFileName = {};
	openFileName.lStructSize = sizeof(OPENFILENAMEW);
	openFileName.nMaxFile = MAX_PATH;//这个必须设置，不设置的话不会出现打开文件对话框  
	openFileName.lpstrFilter = "文本文件（*.mapo）\0*.mapo\0任何文件（*.*）\0*.*\0";
	openFileName.lpstrFile = szFileName;
	openFileName.nFilterIndex = 1;
	openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	char szCurDir[MAX_PATH];
	if(savepath.empty()){
		GetCurrentDirectory(sizeof(szCurDir), szCurDir);
		char* fd = strrchr(szCurDir,'\\');
		fd[0]='\0';
		strcat(szCurDir,"\\Client\\runtime\\res\\map\\*.mapo");
	}
	else{
		strcpy(szCurDir,savepath.c_str());
	}
	openFileName.lpstrFile = szCurDir;

	if (GetSaveFileName(&openFileName))
	{
		m_map.Save(openFileName.lpstrFile);
		m_background.Save(m_map.GetResOut());
	}

}

void CProject::CbRes(){
	CMapinfo& m_map = m_grid;
	m_background.FreeRes();
	m_map.CreatRes();
	SetTimer(LoadBackGround,10);
}