#ifdef _WINDOWS

#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "resource.h"
#include "LogWindow.h"

#include "pl0.h"
#include "Code.h"
#include "Error.h"

char Program[10240];
char Result[1024];
char FileName[1024];

void TextCompile(HWND hWnd)
{
	Result[0] = '\0';
	GetDlgItemText(hWnd,IDC_EDIT1,Program,sizeof(Program));

	GetResult(Program,Result);
	SetDlgItemText(hWnd,IDC_EDIT2,Result);

	puts(Result);

	int i = 0;
	char ErrorStr[128];

	SendDlgItemMessage(hWnd,IDC_LIST1,LB_RESETCONTENT,0,0);

	while (1) {
		if (ErrorGetStr(i,ErrorStr) == 0) break;
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LONG)ErrorStr);
		i++;
	}
}

void FileSave(const char *File)
{
	FILE *fp = fopen(File,"w");
	if (fp == NULL) return;

	fwrite(Program,1,strlen(Program) + 1,fp);
	fclose(fp);
}

void FileRead(const char *File)
{
	size_t size;

	if (File == NULL) return;

	FILE *fp = fopen(File,"rb");
	if (fp == NULL) return;

	size = fread(Program,1,1024,fp);
	fclose(fp);
	Program[size] = '\0';

}

void YrDelete(char *Buf)
{
	int i = 0,j = 0;

	while (Buf[j] != '\0') {
		if (Buf[j] != '\r') {
			Buf[i] = Buf[j];
			i++;
		}
		j++;
	}

	Buf[i] = '\0';
}

void FileOpen(HWND hWnd)
{
	OPENFILENAME ofn = {0};
	char ImageDir[256] = {0};
	char szFileName[MAX_PATH] = {0};
	char FileTitle[64] = {0};
	char WinText[256] = {0};

	ofn.lpstrFilter = "OPL0ソースファイル\0*.PL0\0テキストファイル\0*.TXT\0全てのファイル\0*.*\0";

	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFileName;
	ofn.lpstrInitialDir = ImageDir;
	ofn.lpstrFileTitle = FileTitle;
	ofn.lpstrCustomFilter = NULL;
	ofn.lpstrTitle = NULL;
	ofn.lpstrDefExt = "ST2";
	ofn.lStructSize = sizeof(ofn);
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = 64;
	ofn.nMaxCustFilter = NULL;
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.nFileExtension = NULL;

	if (GetOpenFileName(&ofn)) {
		FileRead(ofn.lpstrFile);
		SetDlgItemText(hWnd,IDC_EDIT3,ofn.lpstrFile);
		SetDlgItemText(hWnd,IDC_EDIT1,Program);
	}
}

void FontChange(HWND hWnd)
{
	LOGFONT LogFont = {0};
	CHOOSEFONT CFont = { sizeof(CHOOSEFONT) };

	LogFont.lfCharSet = SHIFTJIS_CHARSET;

	CFont.hwndOwner = hWnd;
	CFont.lpLogFont = &LogFont;
	CFont.Flags = CF_SCREENFONTS|CF_SELECTSCRIPT|CF_NOVERTFONTS;
	CFont.nFontType = SCREEN_FONTTYPE;
	CFont.lpLogFont->lfWeight = 0;
	CFont.lpLogFont->lfItalic = 0;
	CFont.lpLogFont->lfCharSet = DEFAULT_CHARSET;
	//lstrcpy(CFont.lpLogFont->lfFaceName,);

//	HDC hDC = GetDC(0);
//	CFont.lpLogFont->lfHeight = -MulDiv( ,GetDeviceCaps(hDC, LOGPIXELSY),72);
//	ReleaseDC(0,hDC);

	if (ChooseFont(&CFont) != 0) {

		HDC hDC = GetDC(hWnd);

		int Height = MulDiv(CFont.iPointSize / 10,GetDeviceCaps(hDC,LOGPIXELSY),72);

		HFONT hFont = CreateFont(Height,0,0,0,0,0,
			false,false,SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS,CLIP_CHARACTER_PRECIS,
			ANTIALIASED_QUALITY,FF_DONTCARE | DEFAULT_PITCH ,CFont.lpLogFont->lfFaceName);
		
		ReleaseDC(hWnd,hDC);

		SendDlgItemMessage(hWnd,IDC_EDIT1,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);
		SendDlgItemMessage(hWnd,IDC_EDIT2,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);
		SendDlgItemMessage(hWnd,IDC_LIST1,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);
		LogWinFont(hFont);
	}


}

BOOL CALLBACK MainWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{

	switch (message) 
	{
	case WM_INITDIALOG:
		FileRead(__argv[1]);
		SetDlgItemText(hWnd,IDC_EDIT3,__argv[1]);
		SetDlgItemText(hWnd,IDC_EDIT1,Program);

		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDB_FONT:
			FontChange(hWnd);
			return TRUE;
		case IDB_OPEN:
			FileOpen(hWnd);
			return TRUE;
		case IDB_LIST:
			CodeShow();
			return TRUE;
		case IDB_COM:
			TextCompile(hWnd);
			return TRUE;
		case IDB_SAVE:
			GetDlgItemText(hWnd,IDC_EDIT1,Program,sizeof(Program));
			GetDlgItemText(hWnd,IDC_EDIT3,FileName,sizeof(FileName));
			YrDelete(Program);
			FileSave(FileName);
			return TRUE;
		default:
			return FALSE;
		}
	case WM_CLOSE:
		EndDialog(hWnd,0);
		return TRUE;
	default:
		return FALSE;
	}

}

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow )
{
	setlocale(LC_ALL, "jpn"); // 日本語で処理
	CreateLogWindow(hInstance); LogWinShow();
	DialogBox(hInstance,"IDD_DIALOG1",NULL,(DLGPROC)MainWndProc);

	return 0;
}

#endif