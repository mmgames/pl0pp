#ifndef _WINDOWS

#include <stdio.h>
#include <string.h>
#include <locale.h>

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

int main(int argc,char *argv[])
{
	setlocale(LC_ALL, "jpn");	// システム言語を日本語に
	
	printf("%s 読み込み",argv[1]);
	FILE *fp = fopen(argv[1],"rt");
	fread(Program,1024,1,fp);
	if (fp == NULL) {
		printf("%s 読み込み失敗",argv[1]);
		return 0;
	}
	
	puts("\nプログラム");
	puts(Program);

	GetResult(Program,Result);

	puts("\n実行結果");
	puts(Result);

	fclose(fp);

	int i = 0;
	char ErrorStr[128];

	puts("\nエラー");
	while (1) {
		if (ErrorGetStr(i,ErrorStr) == 0) break;
		puts(ErrorStr);
		i++;
	}

	fflush(stdin);
	getchar();
	
	return 0;
}

#endif