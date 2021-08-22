#include <stdio.h>
#include <stdarg.h>
#include "LogWindow.h"

#ifdef _WINDOWS

static HWND hWnd = NULL;
static HANDLE hLOGFile = NULL;
static bool LogNo = false;

#endif

static char* DirectoryFound(void)
{
	#ifdef _WINDOWS

	static int i;
	static char Buffer[1024];
	
	// カレントディレクトリィの取得
	GetModuleFileName(NULL, Buffer, 256);

	for (i = strlen(Buffer); Buffer[i] != '\\'; i--) Buffer[i] = NULL;

	return Buffer;

	#endif
}

void LogStop(void)
{
	#ifdef _WINDOWS

	LogNo = true;

	#endif
}

#ifdef _WINDOWS
void CreateLogWindow(HINSTANCE hInstance)
{
	

	if (LogNo == true) return;

	hWnd = CreateWindow(
		"LISTBOX",
		"デバッグログ",
		WS_OVERLAPPEDWINDOW|WS_VSCROLL|LBS_NOINTEGRALHEIGHT,
		30,30,640,480,
		NULL,NULL,hInstance,NULL);

	HDC hDC = GetDC(hWnd);
	int Height = MulDiv(10,GetDeviceCaps(hDC,LOGPIXELSY),72);
	ReleaseDC(hWnd,hDC);

	HFONT hFont = CreateFont(Height,0,0,0,0,0,
		false,false,SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS,CLIP_CHARACTER_PRECIS,
		ANTIALIASED_QUALITY,FF_DONTCARE | DEFAULT_PITCH , "ＭＳ ゴシック");

	SendMessage(hWnd, WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);

	
}
#endif

void LogWinFont(HFONT hFont)
{
	SendMessage(hWnd,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);
}

static void OutText(const char buf[])
{	
	#ifdef _WINDOWS

	if (LogNo == true) return;

	int i = SendMessage(hWnd,LB_ADDSTRING,0,(LONG)buf);
	SendMessage(hWnd,LB_SETCURSEL,i,0);
	if (i > 30000) SendMessage(hWnd,LB_RESETCONTENT,0,0);

	if (hLOGFile == NULL) {
		char SFile[MAX_PATH];
		wsprintf(SFile,"%s%s",DirectoryFound(),"LOG.TXT");
		hLOGFile = CreateFile(SFile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS,NULL);
	}

	DWORD dw;
	WriteFile(hLOGFile,buf,lstrlen(buf),&dw,NULL);
	WriteFile(hLOGFile,"\r\n",lstrlen("\r\n"),&dw,NULL);

	#endif
}

void LogWinShow(void)
{
	#ifdef _WINDOWS

	if (LogNo == true) return;

	ShowWindow(hWnd,SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	#endif
}

void LogWinPaint(void)
{
	#ifdef _WINDOWS

	if (LogNo == true) return;

	UpdateWindow(hWnd);

	#endif
}

void TXTLOG(const char format[], ...)
{
	#ifdef _WINDOWS

	if (LogNo == true) return;

	va_list ap;
	char buf[1024];
	
	va_start(ap,format);
   	//vsprintf_s( buf,sizeof(buf), format, ap );
	vsprintf( buf, format, ap );

	OutText(buf);

	va_end(ap);

	#endif
}

#ifdef _WINDOWS

void NumLogWindow(const char name[],DWORD val)
{

	if (LogNo == true) return;

	char buf[1024];

	wsprintf(buf,"%s = %lu",name,val);
	
	OutText(buf);

}

#endif

void NumLogWindow(const char name[],int val)
{
	#ifdef _WINDOWS
	
	if (LogNo == true) return;

	char buf[1024];
	wsprintf(buf,"%s = %d",name,val);

	OutText(buf);
	
	#endif
}

void NumLogWindow(const char name[],double val)
{
	#ifdef _WINDOWS

	if (LogNo == true) return;

	char buf[1024];

	sprintf(buf,"%s = %f",name,val);
	
	OutText(buf);

	#endif
}

void NumLogWindow(const char name[],char val)
{
	#ifdef _WINDOWS

	if (LogNo == true) return;

	char buf[1024];

	sprintf(buf,"%s = %c",name,val);

	OutText(buf);

	#endif
}

void NumLogWindow(const char name[],const char val[])
{
	#ifdef _WINDOWS

	if (LogNo == true) return;

	char buf[1024];

	wsprintf(buf,"%s = %s",name,val);

	OutText(buf);

	#endif
}

void LogLastError(void)
{
	#ifdef _WINDOWS

	if (LogNo == true) return;

	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL);
	OutText((char *)lpMsgBuf);
	LocalFree(lpMsgBuf);

	#endif
}