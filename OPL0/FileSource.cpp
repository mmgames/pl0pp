#include <stdio.h>
#include <string.h>
#include "FileSource.h"
#include "LogWindow.h"

static char Program[1024];
static int Pos,Line,Count;

void FileSourceText(char *Buf)
{
	strcpy(Program,Buf);
	Pos = 0;
	Line = 1;
	Count = 0;

	/*
	int i;
	for (i = 0;Program[i] != '\0';i++) {
	if (Program[i] == '\r' || Program[i] == '\n') Program[i] = ' ';
	}

	*/
}

char FileSourceNextChar(void)
{
	char c = Program[Pos];
	Pos++;

	if (c == '\n') {
		Line++;
		Count = 0;
	} else {
		Count++;
	}

	return c;
}

int FileSourceGetLine(void) { return Line; }
int FileSourceGetCount(void) { return Count; }
