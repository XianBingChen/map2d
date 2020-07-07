// 2DMapMask.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "2DMapMask.h"


#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

HWND g_hWnd;
ULONG_PTR m_pGdiToken;
GdiplusStartupInput m_gdiplusStartupInput;
// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;
	
	WIN32_FIND_DATA  FindFileData;
    BOOL bValue = false;
    HANDLE hFind = FindFirstFile(".\\Source",  &FindFileData);
    if (!((hFind != INVALID_HANDLE_VALUE) && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
    {
		CreateDirectory(".\\Source",NULL);
    }
	SetCurrentDirectory(".\\Source");
    FindClose(hFind);


	GdiplusStartup( &m_pGdiToken, &m_gdiplusStartupInput, NULL );
	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY2DMAPMASK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2DMAPMASK));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	GdiplusShutdown( m_pGdiToken );
	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2DMAPMASK));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MY2DMAPMASK);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//

void MySetTimer(int id,int ts){
	::SetTimer(g_hWnd,id,ts,NULL);
}

void CenterWindow(HWND hWnd)
{
    int scrWidth, scrHeight;
    RECT rect;
    //获得屏幕尺寸
    scrWidth = GetSystemMetrics(SM_CXSCREEN);
    scrHeight = GetSystemMetrics(SM_CYSCREEN);
    //取得窗口尺寸
    GetWindowRect(hWnd, &rect);
    //重新设置rect里的值  
    long width = rect.right - rect.left;
    long height = rect.bottom - rect.top;
    rect.left = (scrWidth - width) / 2;
    rect.top = (scrHeight - height) / 2;
 
    //移动窗口到指定的位置
    SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, width, height, SWP_NOSIZE | SWP_NOZORDER);
}

bool IsDrag=false;
POINT g_pos;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HMENU hm ;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	POINT pos;
	CProject* proj = (CProject*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	GetCursorPos(&pos);
	ScreenToClient(hWnd,&pos);
	switch (message)
	{
	case WM_CREATE:{
			g_hWnd = hWnd;

			MENUBARINFO menuInfo = {0};
			menuInfo.cbSize = sizeof(MENUBARINFO);
			int rtn = GetMenuBarInfo(hWnd, OBJID_MENU, 0, &menuInfo);
			int menu_bar_height = 0;
			if(rtn != 0){
				menu_bar_height = menuInfo.rcBar.bottom - menuInfo.rcBar.top+1;
			}

		   RECT rect;
		   rect.right = int(1280/66)*66+1;
		   rect.bottom = int(720/44)*44+1;
		   rect.left = 0;
		   rect.top = 0;

			CLayer::setVWH(rect.right,rect.bottom);
			CLayer::setCWH(66,44);
			CLayer::setWWH(66*100,44*100);

			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)new CProject(MySetTimer,hWnd,1920,1080));
			::SetTimer(hWnd,1000,10,NULL);

		   AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
		   ::SetWindowPos(hWnd,NULL,0,0,rect.right - rect.left, rect.bottom - rect.top,SWP_NOMOVE);
		   CenterWindow(hWnd);
		}
		break;
	case WM_TIMER:
		if(wParam == 1000){
			RECT rc;
			GetClientRect(hWnd,&rc);
			InvalidateRect(hWnd,&rc,true);
			UpdateWindow(hWnd);
		}
		else{
			::KillTimer(hWnd,wParam);
			proj->OnTimer((int)wParam);
		}
		break;
	case WM_SIZE:
		proj->OnSize(LOWORD( lParam ),HIWORD( lParam ));
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		hm = GetMenu(hWnd);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_LOAD:
			proj->Load();
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_OPEN:
			proj->Open();
			break;
		case IDM_OBSTRUCT:
			proj->ShowBack(false);
			proj->SelectLayer(MAP_BLOCK);			
			::CheckMenuItem(hm,IDM_OBSTRUCT,MF_CHECKED);
			::CheckMenuItem(hm,IDM_SHADE,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_SHOWBACK,MF_UNCHECKED);
			break;
		case IDM_SHADE:
			proj->ShowBack(false);
			proj->SelectLayer(MAP_MARK);		
			::CheckMenuItem(hm,IDM_SHADE,MF_CHECKED);
			::CheckMenuItem(hm,IDM_OBSTRUCT,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_SHOWBACK,MF_UNCHECKED);

			break;
		case IDM_SHOWBACK:
			proj->ShowBack(true);
			proj->SelectLayer(MAP_NORMAL);
			::CheckMenuItem(hm,IDM_SHOWBACK,MF_CHECKED);
			::CheckMenuItem(hm,IDM_OBSTRUCT,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_SHADE,MF_UNCHECKED);
			break;
		case IDM_ERASE:
			proj->DoErase(true);
			::CheckMenuItem(hm,IDM_ERASE,MF_CHECKED);
			::CheckMenuItem(hm,IDM_PEN,MF_UNCHECKED);
			break;
		case IDM_PEN:
			proj->DoErase(false);
			::CheckMenuItem(hm,IDM_PEN,MF_CHECKED);
			::CheckMenuItem(hm,IDM_ERASE,MF_UNCHECKED);
			break;

		case IDM_SCALE1:
			proj->SetScaleXY(1);
			::CheckMenuItem(hm,IDM_SCALE1,MF_CHECKED);
			::CheckMenuItem(hm,IDM_SCALE2,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_SCALE4,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_SCALE8,MF_UNCHECKED);
			break;	

		case IDM_SCALE2:
			proj->SetScaleXY(2);
			::CheckMenuItem(hm,IDM_SCALE1,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_SCALE2,MF_CHECKED);
			::CheckMenuItem(hm,IDM_SCALE4,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_SCALE8,MF_UNCHECKED);
			break;
			
		case IDM_SCALE4:
			proj->SetScaleXY(4);
			::CheckMenuItem(hm,IDM_SCALE1,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_SCALE2,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_SCALE4,MF_CHECKED);
			::CheckMenuItem(hm,IDM_SCALE8,MF_UNCHECKED);
			break;
			
		case IDM_SCALE8:
			proj->Clear();
			proj->SetScaleXY(8);
			::CheckMenuItem(hm,IDM_SCALE1,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_SCALE2,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_SCALE4,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_SCALE8,MF_CHECKED);
			break;

		case IDM_PEN_X1:
			proj->SetPenSize(1);
			::CheckMenuItem(hm,IDM_PEN_X1,MF_CHECKED);
			::CheckMenuItem(hm,IDM_PEN_X2,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_PEN_X3,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_PEN_X4,MF_UNCHECKED);
			break;

		case IDM_PEN_X2:
			proj->SetPenSize(3);
			::CheckMenuItem(hm,IDM_PEN_X1,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_PEN_X2,MF_CHECKED);
			::CheckMenuItem(hm,IDM_PEN_X3,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_PEN_X4,MF_UNCHECKED);
			break;

		case IDM_PEN_X3:
			proj->SetPenSize(5);
			::CheckMenuItem(hm,IDM_PEN_X1,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_PEN_X2,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_PEN_X3,MF_CHECKED);
			::CheckMenuItem(hm,IDM_PEN_X4,MF_UNCHECKED);

		case IDM_PEN_X4:
			proj->SetPenSize(7);
			::CheckMenuItem(hm,IDM_PEN_X1,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_PEN_X2,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_PEN_X3,MF_UNCHECKED);
			::CheckMenuItem(hm,IDM_PEN_X4,MF_CHECKED);
			break;

		case IDM_SVAE:
			proj->Save();
			break;
		case IDM_CBRES:
			proj->CbRes();
			break;
		case IDM_UNDO:
			proj->DoEdit(true);
			break;
		case IDM_REDO:
			proj->DoEdit(false);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_LBUTTONDOWN:
		proj->Clear();
		SetCapture(hWnd);
		proj->OnMouseDown(pos.x,pos.y,(::GetAsyncKeyState(VK_SHIFT)&0xff00)>0);
		break;
	case WM_LBUTTONUP:
		proj->OnMouseUp(LOWORD(lParam),HIWORD(lParam));
		ReleaseCapture();
		break;
	case WM_RBUTTONDOWN:
		IsDrag = true;
		g_pos = pos;
		SetCapture(hWnd);

		proj->Clear();
		proj->StarDrag(pos.x,pos.y);
		break;
	case WM_RBUTTONUP:
		IsDrag = false;
		ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		if(IsDrag){
			proj->OnDrag(pos.x,pos.y,pos.x-g_pos.x,pos.y-g_pos.y);
		}
		else{
			proj->OnMouseMove(pos.x,pos.y);
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_HOME:{			
				RECT rc;
				GetClientRect(hWnd, &rc);
				proj->Home(rc.right-rc.left, rc.bottom-rc.top);
			}
			break;
		case VK_UP:
			proj->Up();
			break; 
		case VK_DOWN:
			proj->Down();
			break; 
		case VK_LEFT:
			proj->Left();
			break; 
		case VK_RIGHT:
			proj->Right();
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_UP:
			proj->Up(0);
			break; 
		case VK_DOWN:
			proj->Down(0);
			break; 
		case VK_LEFT:
			proj->Left(0);
			break; 
		case VK_RIGHT:
			proj->Right(0);
			break;
		}
		break;
	case WM_ERASEBKGND:
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		{
			proj->OnFlresh(hdc);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		if(proj)delete proj;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR, Image* &pImg)
{
    HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type
    if (!hRsrc)
        return FALSE;
    // load resource into memory
    DWORD len = SizeofResource(hInst, hRsrc);
    BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
    if (!lpRsrc)
        return FALSE;
    // Allocate global memory on which to create stream
    HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
    BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
    memcpy(pmem,lpRsrc,len);
    IStream* pstm;
    CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);
    // load from stream
    pImg=Gdiplus::Image::FromStream(pstm);
    // free/release stuff
    GlobalUnlock(m_hMem);
    pstm->Release();
    FreeResource(lpRsrc);
    return TRUE;
}
