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
	setlocale(LC_ALL, "jpn");	// �V�X�e���������{���
	
	printf("%s �ǂݍ���",argv[1]);
	FILE *fp = fopen(argv[1],"rt");
	fread(Program,1024,1,fp);
	if (fp == NULL) {
		printf("%s �ǂݍ��ݎ��s",argv[1]);
		return 0;
	}
	
	puts("\n�v���O����");
	puts(Program);

	GetResult(Program,Result);

	puts("\n���s����");
	puts(Result);

	fclose(fp);

	int i = 0;
	char ErrorStr[128];

	puts("\n�G���[");
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