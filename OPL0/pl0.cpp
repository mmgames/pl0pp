#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "FileSource.h"
#include "Token.h"
#include "Grammar.h"
#include "Code.h"
#include "Error.h"
#include "LogWindow.h"

int GetResult(char *Program,char *Result)
{	
	setlocale(LC_ALL, "jpn"); // ���{��ŏ���

	// �\�[�X�t�@�C���̓ǂݍ���
	FileSourceText(Program);

	// �R���p�C���̎��s
	// �����́i�P��̊i�[�j
	TokenAll();

	// �\����́��ړI�ꐶ���i�@�B��̐����j
	try {
		ErrorInit();
		GrammerAll();
		CodeShow();
	} catch(...) {
		TXTLOG("�����s���G���[�ɑ����������߁A�R���p�C�����������~���܂��B");
		return -1;
	}

	// �C���^�v���^�̎��s
	try {
		Interpreter(Result,0);
	} catch(...) {
		TXTLOG("�C���^�v���^���s���ɗ�O�����B�v���O�����ɒv���I�ȃo�O������Ǝv���܂��B");
		return -1;
	}

	return 0;
}

// �G���[���b�Z�[�W�̎擾
int GetErrorText(char *Error)
{
	int i = 0;
	char ErrorStr[128];

	try {
		while (1) {
			if (ErrorGetStr(i,ErrorStr) == 0) break;
			strcat(Error,ErrorStr);
			i++;
		}
	} catch(...) {
		return i;
	}

	return i;
}