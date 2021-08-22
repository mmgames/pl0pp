#ifndef __INCLUDE_LOGWINDOW__
#define __INCLUDE_LOGWINDOW__

#ifdef _WINDOWS

#include <Windows.h>
void CreateLogWindow(HINSTANCE hInstance);
void NumLogWindow(const char name[],DWORD val);
void LogWinFont(HFONT hFont);
#endif

void LogStop(void);
void NumLogWindow(const char name[],int val);
void NumLogWindow(const char name[],double val);
void NumLogWindow(const char name[],char val);
void NumLogWindow(const char name[],const char val[]);
void LogWinPaint(void);
void LogWinShow(void);
void LogLastError(void);

void TXTLOG(const char format[], ...);
#define NUMLOG(VAL) NumLogWindow(#VAL,VAL)

#endif