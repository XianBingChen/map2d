// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��
#include <atlcomcli.h>

// c++ ��׼��
#include <string>
#include <vector>
#include <map>
using namespace std;

//win32 ��
#include <comdef.h>
#include <tchar.h>
#include <commdlg.h>

//ͼ�ο�
#include "GdiPlus.h"
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

#include "DataRecord.h"
#include "Layer.h"
#include "MiniMap.h"
#include "BackGround.h"
#include "DrawView.h"
#include "DrawGrid.h"
#include "Project.h"


void OutputDebugPrintf(const char* strOutputString,...);
BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR, Image* &pImg);