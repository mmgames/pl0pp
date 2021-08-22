#include <stdio.h>
#include <string.h>
#include "Code.h"
#include "LogWindow.h"

void Interpreter(char *Result,int StackView)
{
	int Stack[1024] = {0};		// ���Z�p�X�^�b�N
	int PCStack[1024] = {0};	// ���^�[���A�h���X�X�^�b�N
	int Memory[1024] = {0};		// �ėp�������i�ϐ��L����j
	int PC, CPC,StackPos,PCStackPos;
	char Buf[256] = {0};
	
	PC = StackPos = PCStackPos = 0;
	
	if (StackView) TXTLOG("���s�X�^�b�N�\��");

	do {

		CPC = PC;
		PC++;
		
		switch (Code[CPC].VCode)
		{
		case VC_SAVE:
			if (Code[CPC].DataA == STOCK) {
				// �X�^�b�N�ɋL��
				StackPos++;
				Stack[StackPos] = Code[CPC].DataB;
			} else {
				// �ėp�������ɋL��
				Memory[Code[CPC].DataA] = Code[CPC].DataB;
			}
			break;
		case VC_COPY:
			if (Code[CPC].DataA == STOCK) {
				// �X�^�b�N����ėp������
				Memory[Code[CPC].DataB] = Stack[StackPos];
				StackPos--;
			} else if (Code[CPC].DataB == STOCK) {
				// �ėp����������X�^�b�N
				StackPos++;
				Stack[StackPos] = Memory[Code[CPC].DataA];
			} else {
				// ���������m
				Memory[Code[CPC].DataB] = Memory[Code[CPC].DataA];
			}
			break;
		case VC_POUT: // �|�C���^����X�^�b�N
			if (Code[CPC].DataA == STOCK) {
				Stack[StackPos] = Memory[Stack[StackPos]];
			} else {
				StackPos++;
				Stack[StackPos] = Memory[Memory[Code[CPC].DataA]];
			}
			break;
		case VC_PINT: // �X�^�b�N����|�C���^
			if (Code[CPC].DataA == STOCK) {
				Memory[Stack[StackPos - 1]] = Stack[StackPos];
				StackPos-=2;
			} else {
				Memory[Memory[Code[CPC].DataA]] = Stack[StackPos];
				StackPos--;
			}
			break;
		case VC_JUMP:
			PC = Code[CPC].DataA;
			break;
		case VC_JUMP0:
			if (Stack[StackPos] == 0) PC = Code[CPC].DataA;
			StackPos--;
			break;
		case VC_END:
			return;
		case VC_PRINT:
			sprintf(Buf,"%d ", Stack[StackPos]);
			strcat(Result,Buf);
			StackPos--;
			break;
		case VC_NEWLINE:
			strcat(Result,"\r\n");
			break;
		case VC_NON:
			break;
		case VC_CALL:
			PCStack[PCStackPos] = PC;
			PCStackPos++;
			PC = Code[CPC].DataA;
			break;
		case VC_RETURN:
			PCStackPos--;
			PC = PCStack[PCStackPos];
			break;
		case VC_CALC:
			switch (Code[CPC].DataA) {
			case OR_NEG:
				Stack[StackPos] = -Stack[StackPos];
				break;
			case OR_ADD:
				Stack[StackPos - 1] += Stack[StackPos];
				StackPos--;
				break;
			case OR_SUB:
				Stack[StackPos - 1] -= Stack[StackPos];
				StackPos--;
				break;
			case OR_MUL:
				Stack[StackPos - 1] *= Stack[StackPos];
				StackPos--;
				break;
			case OR_DIV:
				Stack[StackPos - 1] /= Stack[StackPos];
				StackPos--;
				break;
			case OR_ODD:
				Stack[StackPos] = Stack[StackPos] & 1;
				break;
			case OR_EQ:
				Stack[StackPos - 1] = (Stack[StackPos-1] == Stack[StackPos]);
				StackPos--;
				break;
			case OR_LS:
				Stack[StackPos - 1] = (Stack[StackPos-1] < Stack[StackPos]);
				StackPos--;
				break;
			case OR_GR:
				Stack[StackPos - 1] = (Stack[StackPos-1] > Stack[StackPos]);
				StackPos--;
				break;
			case OR_NEQ:
				Stack[StackPos - 1] = (Stack[StackPos-1] != Stack[StackPos]);
				StackPos--;
				break;
			case OR_LSEQ:
				Stack[StackPos - 1] = (Stack[StackPos-1] <= Stack[StackPos]);
				StackPos--;
				break;
			case OR_GREQ:
				Stack[StackPos - 1] = (Stack[StackPos-1] >= Stack[StackPos]);
				StackPos--;
				break;
			}
			break;
		}

	if (StackView) {
		char Buf[256] = {0};
		int i;
		sprintf(Buf,"[%3d]",CPC);
		for (i = 0;i <= StackPos;i++) {
			sprintf(Buf,"%s %d",Buf,Stack[i]);
		}
		TXTLOG(Buf);
	}

	} while (1);

}