#include <stdio.h>
#include <string.h>
#include "Error.h"
#include "LogWindow.h"

static int Err;
static char Error[ERROR_MAX][128];

// エラーを初期化
void ErrorInit(void)
{
	Err = 0;
	memset(Error,0,sizeof(Error));
}

// エラー内容をファイルに書き出す
int ErrorWriteFile(const char *File)
{
	int i;

	FILE *fp = fopen(File,"w");
	if (fp == NULL) return 0;

	for (i = 0;i < Err;i++) {
		fputs(Error[i],fp);
	}

	fclose(fp);

	return 1;
}

// エラー文字列を取得
int ErrorGetStr(int No,char *Buf)
{
	if (No >= Err) return 0;
	strcpy(Buf,Error[No]);
	return 1;
}

// 新しいエラーを記憶
int ErrorSet(ERRORTYPE No,int Line,int Count,const char *s1,const char *s2,int i1,int i2)
{
	sprintf(Error[Err],"%3d行%3d字：",Line,Count);

	switch (No) {

	case ERROR_NONAME: sprintf(Error[Err],"%s %s 名前ではありません。",Error[Err],s1); break;
	case ERROR_NUMBER: sprintf(Error[Err],"%s %s 数値ではありません。",Error[Err],s1); break;
	case ERROR_EQUAL: sprintf(Error[Err],"%s = が不足しています。",Error[Err]); break;
	case ERROR_SEMICOLON: sprintf(Error[Err],"%s ; が不足しています。",Error[Err]); break;
	case ERROR_DO: sprintf(Error[Err],"%s do が不足しています。",Error[Err]); break;
	case ERROR_THEN: sprintf(Error[Err],"%s then が不足しています。",Error[Err]); break;
	case ERROR_LPAREN: sprintf(Error[Err],"%s ( が不足しています。",Error[Err]); break;
	case ERROR_RPAREN: sprintf(Error[Err],"%s ) が不足しています。",Error[Err]); break;
	case ERROR_LARRAY: sprintf(Error[Err],"%s [ が不足しています。",Error[Err]); break;
	case ERROR_RARRAY: sprintf(Error[Err],"%s ] が不足しています。",Error[Err]); break;
	case ERROR_COMMA: sprintf(Error[Err],"%s , が不足しています。",Error[Err]); break;
	case ERROR_PERIOD: sprintf(Error[Err],"%s . が不足しています。",Error[Err]); break;

	case ERROR_TOKEN: sprintf(Error[Err],"%s %s は無意味な単語です。",Error[Err],s1); break;
	case ERROR_CONDITION: sprintf(Error[Err],"%s %s は条件演算子ではありません。",Error[Err],s1); break;
	case ERROR_NAME: sprintf(Error[Err],"%s %s 宣言されていない名前です。",Error[Err],s1); break;
	case ERROR_CONST: sprintf(Error[Err],"%s %s 定数には代入できません。",Error[Err],s1); break;
	case ERROR_CALCADDRESS: sprintf(Error[Err],"%s %s のアドレスは計算できません。",Error[Err],s1); break;
	case ERROR_POINTER: sprintf(Error[Err],"%s %s はポインタ変数ではありません。",Error[Err],s1); break;
	case ERROR_ARRAY: sprintf(Error[Err],"%s %s は配列名ではありません。",Error[Err],s1); break;
	case ERROR_ARRAYINPUT: sprintf(Error[Err],"%s %s 配列名には代入できません。",Error[Err],s1); break;
	case ERROR_TYPE: sprintf(Error[Err],"%s %s 異なる型同士では代入できません。",Error[Err],s1); break;

	default: sprintf(Error[Err],"%s 原因不明エラー",Error[Err]); break;

	}

	Err++;

	return 0;
}

