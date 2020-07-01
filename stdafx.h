// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的
#include <atlcomcli.h>

// c++ 标准库
#include <string>
#include <vector>
#include <map>
using namespace std;

//win32 库
#include <comdef.h>
#include <tchar.h>
#include <commdlg.h>

//图形库
#include "GdiPlus.h"
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

// TODO: 在此处引用程序需要的其他头文件

#include "DataRecord.h"
#include "Layer.h"
#include "MiniMap.h"
#include "BackGround.h"
#include "DrawView.h"
#include "DrawGrid.h"
#include "Project.h"


void OutputDebugPrintf(const char* strOutputString,...);
BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR, Image* &pImg);