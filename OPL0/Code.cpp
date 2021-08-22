#include <stdio.h>
#include <string.h>
#include "Code.h"
#include "Table.h"
#include "LogWindow.h"

static int CodePos = -1;
CodeInfo Code[MIDDLECODE_MAX];

// ������
void CodeInit(void)
{
	CodePos = -1;
	memset(Code,0,sizeof(Code));
}

// ���O�ɒǉ��������߂̃A�h���X
int GetCodePos(void) { return CodePos; }

// �V�������߂�ǉ�
void CodeAdd(VirtualCode VCode,int DataA,int DataB)
{
	CodePos++;
	Code[CodePos].VCode = VCode;
	Code[CodePos].DataA = DataA;
	Code[CodePos].DataB = DataB;
}

// �����̖��߂��C������(�o�b�N�p�b�`)
void CodeRevision(int No,int DataA,int DataB)
{
	Code[No].DataA = DataA;
	Code[No].DataB = DataB;

	// ���Ӗ��ȃW�����v���߂��폜
	if (Code[No].VCode == VC_JUMP && Code[No].DataA == No + 1) {
		Code[No].VCode = VC_NON;
		Code[No].DataA = 0;
		Code[No].DataB = 0;
	}
}

// �e���߂̕�������擾
inline void CodeStr(VirtualCode VCode,char *Str)
{
	switch (VCode)
	{
		case VC_SAVE: strcpy(Str,"�L��"); return;
		case VC_COPY: strcpy(Str,"�R�s�["); return;
		case VC_POUT: strcpy(Str,"�Q�Əo��"); return;
		case VC_PINT: strcpy(Str,"�Q�Ɠ���"); return;
		case VC_CALC: strcpy(Str,"�v�Z"); return;
		case VC_PRINT: strcpy(Str,"�\��"); return;
		case VC_NEWLINE: strcpy(Str,"���s"); return;
		case VC_CALL: strcpy(Str,"�֐��ďo"); return;
		case VC_RETURN: strcpy(Str,"�֐����A"); return;
		case VC_JUMP: strcpy(Str,"�W�����v"); return;
		case VC_JUMP0: strcpy(Str,"�O�W�����v"); return;
		case VC_NON: strcpy(Str,"�������Ȃ�"); return;
		case VC_END: strcpy(Str,"�I��"); return;
	}
}

// �e���Z�q�̕�������擾
inline void OperStr(int Oper,char *Str)
{	switch (Oper)
	{
		case OR_NEG: strcpy(Str,"���]"); return;
		case OR_ADD: strcpy(Str,"�{"); return;
		case OR_SUB: strcpy(Str,"�|"); return;
		case OR_MUL: strcpy(Str,"�w"); return;
		case OR_DIV: strcpy(Str,"��"); return;

		case OR_ODD: strcpy(Str,"�"); return;
		case OR_EQ: strcpy(Str,"��"); return;
		case OR_LS: strcpy(Str,"��"); return;
		case OR_GR: strcpy(Str,"��"); return;
		case OR_NEQ: strcpy(Str,"�I��"); return;
		case OR_LSEQ: strcpy(Str,"����"); return;
		case OR_GREQ: strcpy(Str,"����"); return;
	}
}

// ���߃��X�g��\��
void CodeShow()
{
	int i;
	char SCode[256],SA[256],SB[256];

	for (i = 0;i <= CodePos;i++)
	{
		switch (Code[i].VCode) {

			case VC_POUT:
			case VC_PINT:
			case VC_SAVE:
				if (Code[i].DataA == STOCK) strcpy(SA,"�X�^�b�N"); else sprintf(SA,"(%d)",Code[i].DataA);
				sprintf(SB,"%d",Code[i].DataB);
				break;
			case VC_COPY:
				if (Code[i].DataA == STOCK) strcpy(SA,"�X�^�b�N"); else sprintf(SA,"(%d)",Code[i].DataA);
				if (Code[i].DataB == STOCK) strcpy(SB,"�X�^�b�N"); else sprintf(SB,"(%d)",Code[i].DataB);
				break;
			case VC_CALC:
				OperStr(Code[i].DataA,SA);
				SB[0] = '\0';
				break;
			case VC_JUMP:
			case VC_JUMP0:
			case VC_CALL:
				sprintf(SA,"[%d]",Code[i].DataA);
				SB[0] = '\0';
				break;
			case VC_RETURN:
			case VC_PRINT:
			case VC_NEWLINE:
			case VC_NON:
			case VC_END:
				SA[0] = '\0';
				SB[0] = '\0';
				break;

		}

		CodeStr(Code[i].VCode,SCode);
		TXTLOG("[%3d]%11s %10s %10s",i,SCode,SA,SB);
	}
}