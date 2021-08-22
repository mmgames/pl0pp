#include <stdio.h>
#include <string.h>
#include "Error.h"
#include "LogWindow.h"

static int Err;
static char Error[ERROR_MAX][128];

// �G���[��������
void ErrorInit(void)
{
	Err = 0;
	memset(Error,0,sizeof(Error));
}

// �G���[���e���t�@�C���ɏ����o��
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

// �G���[��������擾
int ErrorGetStr(int No,char *Buf)
{
	if (No >= Err) return 0;
	strcpy(Buf,Error[No]);
	return 1;
}

// �V�����G���[���L��
int ErrorSet(ERRORTYPE No,int Line,int Count,const char *s1,const char *s2,int i1,int i2)
{
	sprintf(Error[Err],"%3d�s%3d���F",Line,Count);

	switch (No) {

	case ERROR_NONAME: sprintf(Error[Err],"%s %s ���O�ł͂���܂���B",Error[Err],s1); break;
	case ERROR_NUMBER: sprintf(Error[Err],"%s %s ���l�ł͂���܂���B",Error[Err],s1); break;
	case ERROR_EQUAL: sprintf(Error[Err],"%s = ���s�����Ă��܂��B",Error[Err]); break;
	case ERROR_SEMICOLON: sprintf(Error[Err],"%s ; ���s�����Ă��܂��B",Error[Err]); break;
	case ERROR_DO: sprintf(Error[Err],"%s do ���s�����Ă��܂��B",Error[Err]); break;
	case ERROR_THEN: sprintf(Error[Err],"%s then ���s�����Ă��܂��B",Error[Err]); break;
	case ERROR_LPAREN: sprintf(Error[Err],"%s ( ���s�����Ă��܂��B",Error[Err]); break;
	case ERROR_RPAREN: sprintf(Error[Err],"%s ) ���s�����Ă��܂��B",Error[Err]); break;
	case ERROR_LARRAY: sprintf(Error[Err],"%s [ ���s�����Ă��܂��B",Error[Err]); break;
	case ERROR_RARRAY: sprintf(Error[Err],"%s ] ���s�����Ă��܂��B",Error[Err]); break;
	case ERROR_COMMA: sprintf(Error[Err],"%s , ���s�����Ă��܂��B",Error[Err]); break;
	case ERROR_PERIOD: sprintf(Error[Err],"%s . ���s�����Ă��܂��B",Error[Err]); break;

	case ERROR_TOKEN: sprintf(Error[Err],"%s %s �͖��Ӗ��ȒP��ł��B",Error[Err],s1); break;
	case ERROR_CONDITION: sprintf(Error[Err],"%s %s �͏������Z�q�ł͂���܂���B",Error[Err],s1); break;
	case ERROR_NAME: sprintf(Error[Err],"%s %s �錾����Ă��Ȃ����O�ł��B",Error[Err],s1); break;
	case ERROR_CONST: sprintf(Error[Err],"%s %s �萔�ɂ͑���ł��܂���B",Error[Err],s1); break;
	case ERROR_CALCADDRESS: sprintf(Error[Err],"%s %s �̃A�h���X�͌v�Z�ł��܂���B",Error[Err],s1); break;
	case ERROR_POINTER: sprintf(Error[Err],"%s %s �̓|�C���^�ϐ��ł͂���܂���B",Error[Err],s1); break;
	case ERROR_ARRAY: sprintf(Error[Err],"%s %s �͔z�񖼂ł͂���܂���B",Error[Err],s1); break;
	case ERROR_ARRAYINPUT: sprintf(Error[Err],"%s %s �z�񖼂ɂ͑���ł��܂���B",Error[Err],s1); break;
	case ERROR_TYPE: sprintf(Error[Err],"%s %s �قȂ�^���m�ł͑���ł��܂���B",Error[Err],s1); break;

	default: sprintf(Error[Err],"%s �����s���G���[",Error[Err]); break;

	}

	Err++;

	return 0;
}

