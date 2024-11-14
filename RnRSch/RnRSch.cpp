// RnRSch.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "RnRSch.h"
#include <ShellAPI.h>
#include <iostream>


#define MAX_LOADSTRING 100

struct cryptTable
{
	int codes[55];
	int hash;
	int count;
};

cryptTable gCryptTable;

void createTable()
{
	memset(&gCryptTable, 0, sizeof(gCryptTable));
	int v14 = 0x5CB0;
	gCryptTable.codes[54] = v14;
	int v15 = 0;
	int v7 = 1;
	do
	{
		int v8 = v15 % 0x37;
		gCryptTable.codes[v8] = v7;
		v7 = v14 - v7;
		v15 += 0x15;
		v14 = gCryptTable.codes[v8];
	} while (v15 < 0x46E);

	int v16 = 4;
	do
	{
		int v9 = 0;
		int *v10 = gCryptTable.codes;
		do
		{
			*v10 = *v10 - gCryptTable.codes[(v9++ + 31) % 0x37];
			v10++;
		} while (v9 < 0x37);
		--v16;
	} while (v16);

	gCryptTable.hash = 0;
	gCryptTable.count = 31;
}

void decryptFile(char *fileName)
{
	createTable();
	char DstBuf[4096];
	char fileBuf[1024];
	memset(fileBuf, 0, 1024);
	strcpy(fileBuf, fileName);
	strcat(fileBuf, ".txt");
	memset(DstBuf, 0, 4096);

	char buf[4];
	FILE *f = fopen(fileName, "rb");
	if (!f)
		return;

	fread(buf, 4, 1, f);
	if (strncmp(buf, "SDTE", 4))
	{
		fclose(f);
		return;
	}

	FILE *fOut = fopen(fileBuf, "wb");
	if (!fOut)
	{
		fclose(f);
		return;
	}

	int v3 = fread(DstBuf, 1u, 0x1000u, f);
	while (v3 > 0)
	{
		int v5 = 0;
		int v6 = gCryptTable.hash;
		do
		{
			int v7 = (v6 + 1) % 0x37u;
			int v8 = (gCryptTable.count + 1) % 0x37u;
			gCryptTable.hash = v7;
			gCryptTable.count = v8;
			gCryptTable.codes[v7] -= gCryptTable.codes[v8];
			v6 = gCryptTable.hash;
			DstBuf[v5] ^= LOBYTE(gCryptTable.codes[v6]);
			++v5;
		} while (v5 < v3);
		fwrite(DstBuf, 1u, v3, fOut);
		v3 = fread(DstBuf, 1u, 0x1000u, f);
	}
	fclose(fOut);
	fclose(f);
}

void cryptFile(char *fileName)
{
	createTable();
	char DstBuf[4096];
	char fileBuf[1024];
	memset(fileBuf, 0, 1024);
	memset(DstBuf, 0, 4096);
	if (strncmp(&fileName[strlen(fileName) - 4], ".txt", 4))
		return;
	strcpy(fileBuf, fileName);
	fileBuf[strlen(fileBuf) - 4] = 0;

	FILE *f = fopen(fileName, "rb");
	if (!f)
		return;

	FILE *fOut = fopen(fileBuf, "wb");
	if (!fOut)
	{
		fclose(f);
		return;
	}

	char buf[4];
	buf[0] = 'S';
	buf[1] = 'D';
	buf[2] = 'T';
	buf[3] = 'E';

	fwrite(buf, 4, 1, fOut);

	int v3 = fread(DstBuf, 1u, 0x1000u, f);
	while (v3 > 0)
	{
		int v5 = 0;
		int v6 = gCryptTable.hash;
		do
		{
			int v7 = (v6 + 1) % 0x37u;
			int v8 = (gCryptTable.count + 1) % 0x37u;
			gCryptTable.hash = v7;
			gCryptTable.count = v8;
			gCryptTable.codes[v7] -= gCryptTable.codes[v8];
			v6 = gCryptTable.hash;
			DstBuf[v5] ^= LOBYTE(gCryptTable.codes[v6]);
			++v5;
		} while (v5 < v3);
		fwrite(DstBuf, 1u, v3, fOut);
		v3 = fread(DstBuf, 1u, 0x1000u, f);
	}
	fclose(f);
	fclose(fOut);
}

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр

// Отправить объявления функций, включенных в этот модуль кода:
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
	//createTable();
	MSG msg = { 0 };

	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	while (msg.message != WM_QUIT) 
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= L"RnRSchWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   hWnd = CreateWindow(L"RnRSchWindowClass", L"RnR Sch Extractor", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, 350, 170, NULL, NULL, hInstance, NULL);

   DragAcceptFiles(hWnd, TRUE);
   HFONT font = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
	   OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
	   DEFAULT_PITCH | FF_DONTCARE, L"Tahoma");

   HWND _static = CreateWindow(L"STATIC", L"Для расшифровки SCH или CNF файлов, перетащите их в окно программы. Рядом с файлами появятся расшифрованные текстовые файлы. После их редактирования, перетащите текстовые файлы снова в окно программы, чтобы сконвертировать их обратно.\r\n\r\nБудьте внимательны, файлы заменяются без подтверждения.\r\n\r\nСпециально для vk.com/db_games", WS_VISIBLE | WS_CHILD | SS_LEFT, 5, 5, 330, 140, hWnd, (HMENU)0, NULL, NULL);
   SendMessage(_static, WM_SETFONT, (WPARAM)font, TRUE);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DROPFILES:
	{
		HDROP fDrop = (HDROP)wParam;
		char fName[512];
		int count = DragQueryFileA(fDrop, -1, 0, 0);
		for (int i = 0; i < count; i++)
		{
			 DragQueryFileA(fDrop, i, fName, 512);
			 if (strncmp(&fName[strlen(fName) - 4], ".txt", 4))
				 decryptFile(fName);
			 else
				 cryptFile(fName);
		}
		DragFinish(fDrop);
		break;
	}
	
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
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
