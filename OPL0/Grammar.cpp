#include <stdio.h>
#include <string.h>
#include "Token.h"
#include "Grammar.h"
#include "Statement.h"
#include "Table.h"
#include "Code.h"
#include "Error.h"
#include "LogWindow.h"

static int FuncBlock = 0;			// �֐��u���b�N
static int FuncBlockCode[30];		// �錾���ɍ쐬�����R�[�h�T�C�Y
static int FuncBlockDest[30];		// �u���b�N���̃f�X�g���N�^�I�u�W�F�N�g�̐�
static char FuncBlockDestName[30][30][MAXNAME];		// �u���b�N���̃f�X�g���N�^�̃I�u�W�F�N�g��

static void Block(void);			// �u���b�N�̉��
static void VarDecl(void);	// �ϐ��錾
static void ConstDecl(void);		// �萔�錾
static void FuncDecl(void);			// �֐��錾
static void ClassDecl(void);		// �N���X�錾
static void ClassFuncDecl(void);	// �N���X���֐��錾
static void ObjectDecl(void);		// �I�u�W�F�N�g�錾

static void ConstRactRun(char *ObjectName); // �R���X�g���N�^���s
static void DestRactRun(char *ObjectName); // �f�X�g���N�^���s
static int VarRead(char *Name,int *Pointer,int *Array); // �ϐ��錾��ǂݎ��

int GrammerAll(void)
{
	TNO = 0;
	CodeInit();
	TableInit();

	TableBlockBegin();

	Block();
	CodeAdd(VC_END,0,0);

	return 0;
}

// �u���b�N�̉��
void Block(void)
{
	CodeAdd(VC_JUMP,0,0);
	int JumpAddress = GetCodePos();

	// �錾�����
	while (1)
	{
		switch (Token[TNO].Kind)
		{
		case TK_VAR: // �ϐ��錾
			TNO++;
			VarDecl();
			continue;
		case TK_CONST: // �萔�錾
			TNO++;
			ConstDecl();
			continue;
		case TK_FUNC: // �֐��錾
			FuncBlock++;
			TNO++;
			FuncDecl();
			continue;
		case TK_CLASS: // �N���X�錾
			TNO++;
			ClassDecl();
			continue;
		case TK_OBJECT: // �I�u�W�F�N�g�錾
			TNO++;
			ObjectDecl();
			continue;
		case TK_SEMICOLON:
		case TK_PERIOD:
			TNO++;
			continue;
		default:
			break; // switch�E�o
		}
		break; // while�E�o
	}		

	CodeRevision(JumpAddress,GetCodePos() + 1 - FuncBlockCode[FuncBlock],0);
	Statement(); // �����
	FuncBlockCode[FuncBlock] = 0;

	// �f�X�g���N�^����
	int i;
	for (i = 0;i < FuncBlockDest[FuncBlock];i++) {
		DestRactRun(FuncBlockDestName[FuncBlock][i]);
	}
	FuncBlockDest[FuncBlock] = 0;

	// �֐����A
	if (FuncBlock > 0) {
		FuncBlock--;
		if (Code[GetCodePos()].VCode != VC_RETURN) CodeAdd(VC_RETURN,0,0);
	}

	CodeAdd(VC_NON,0,0);
}

// �ϐ��錾(var i,j;)
void VarDecl(void)
{
	// �ϐ��ǂݎ��
	char Name[1024];
	int End,Pointer,Array;

	do {
		End = VarRead(Name,&Pointer,&Array);
		TableAdd(NK_VAR,Name,Pointer);
		TableAddArray(Array);
	} while (End);

	CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);
}

// �R���X�g���N�^
void ConstRactRun(char *ObjectName)
{
	char FuncName[1024];

	NameInfo ObjName = TableGetNameInfo(ObjectName);
	ClassInfo ClassData = TableGetClassInfo(ObjName.Class);
	sprintf(FuncName,"%s_%s",ClassData.Name,ClassData.Name);
	CodeAdd(VC_SAVE,OBJADDRESS,ObjName.Address);

	NameInfo Name = TableGetNameInfo(FuncName);

	CodeAdd(VC_CALL,Name.Address,0);

	FuncBlockCode[FuncBlock] += 2;
}

// �f�X�g���N�^
void DestRactRun(char *ObjectName)
{
	char FuncName[1024];

	NameInfo ObjName = TableGetNameInfo(ObjectName);
	ClassInfo ClassData = TableGetClassInfo(ObjName.Class);
	sprintf(FuncName,"%s_~%s",ClassData.Name,ClassData.Name);
	CodeAdd(VC_SAVE,OBJADDRESS,ObjName.Address);

	NameInfo Name = TableGetNameInfo(FuncName);

	CodeAdd(VC_CALL,Name.Address,0);
}

// �I�u�W�F�N�g�錾
void ObjectDecl(void)
{
	int ClassNo;
	int ConstRact = 0;
	int DestRact = 0;
	NameInfo Name;

	if (Token[TNO].Kind == TK_NAME) {
		ClassNo = TableAddVarClass(Token[TNO].Name); // ����錾����ϐ������̃N���X�̌^�Ɏw��
		if (ClassNo == 0) ErrorSet(ERROR_NAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		TNO++;
	} else {
		ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
	}

	// �R���X�g���N�^����
	char FuncName[1024];
	ClassInfo ClassData = TableGetClassInfo(ClassNo);
	sprintf(FuncName,"%s_%s",ClassData.Name,ClassData.Name);
	if (TableGetNameNo(FuncName) != -1) {
		Name = TableGetNameInfo(ClassData.Name);
		ConstRact = 1;
	}

	// �f�X�g���N�^����
	sprintf(FuncName,"%s_~%s",ClassData.Name,ClassData.Name);
	if (TableGetNameNo(FuncName) != -1) {
		Name = TableGetNameInfo(ClassData.Name);
		DestRact = 1;
	}

	// �ϐ��ǂݎ��
	char sName[1024];
	int End,Pointer,Array;

	do {

		End = VarRead(sName,&Pointer,&Array);
		TableAdd(NK_VAR,sName,Pointer);
		TableAddArray(Array);

		if (ConstRact == 1) ConstRactRun(sName);
		if (DestRact == 1) {
			strcpy(FuncBlockDestName[FuncBlock][FuncBlockDest[FuncBlock]],sName);
			FuncBlockDest[FuncBlock]++;
		}

	} while (End);

	CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);

	TableAddVarClass(NULL); // �N���X�^�w�������
}

// �萔�錾(const m = 10,n = 5;)
void ConstDecl(void)
{
	int NameNo;

	while (1) {

		if (Token[TNO].Kind == TK_NAME) {

			NameNo = TNO;
			TNO++;

			CheckToken(TK_EQUAL,ERROR_EQUAL);

			if (Token[TNO].Kind == TK_NUMBER) {
				// �萔�o�^
				TableAdd(NK_CONST,Token[NameNo].Name,Token[TNO].Value);
				TNO++;
			} else {
				ErrorSet(ERROR_NUMBER,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
			}

		} else {
			ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		}

		// �J���}�`�F�b�N
		if (Token[TNO].Kind != TK_COMMA) {
			if (Token[TNO].Kind == TK_NAME){
				ErrorSet(ERROR_COMMA,Token[TNO].Line,Token[TNO].Count,NULL,NULL,0,0);
				continue;
			} else {
				break;
			}
		}

		TNO++;

	}

	CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);

}

// �֐��錾(function func(x,y)
void FuncDecl(void)
{
	if (Token[TNO].Kind != TK_NAME) {
		ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		return;
	}

	// �N���X���
	char ClassName[1024];
	char FuncName[1024];
	strcpy(ClassName,Token[TNO].Name);

	char *Class = strchr(ClassName,'_');
	if (Class != NULL) {
		*Class = '\0';
		strcpy(FuncName,&Token[TNO].Name[strlen(ClassName) + 1]);
	}

	// �֐��o�^
	int FuncNameNo = TableAdd(NK_FUNC,Token[TNO].Name,GetCodePos() + 1);

	TNO++;
	CheckToken(TK_LPAREN,ERROR_LPAREN);

	TableBlockBegin();

	// �p�����[�^�ǂݎ��
	if (Token[TNO].Kind != TK_RPAREN) {

		char Name[1024];
		int End,Pointer,Array;

		do {
			End = VarRead(Name,&Pointer,&Array);
			TableFuncParAddress(FuncNameNo,TableAdd(NK_VAR,Name,Pointer));
		} while (End);
	}

	CheckToken(TK_RPAREN,ERROR_RPAREN);

	if (Class != NULL) ClassFuncNo = TableGetClassNo(ClassName);
	Block();
	ClassFuncNo = 0;

	CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);
}

// �N���X�錾
void ClassDecl()
{
	int FuncMode = 0;
	int ClassNo;

	if (Token[TNO].Kind == TK_NAME) {
		
		ClassNo = TableAdd(NK_CLASS,Token[TNO].Name,0);
		TNO++;
		
		while(1) {
			
			if (Token[TNO].Kind == TK_ENDTYPE) {
				TNO++;
				CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);
				return;
			}

			if (Token[TNO].Kind == TK_FUNC) {
				FuncMode = 1;
				TNO++;
				CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);
				continue;
			}

			// �����o�[�ǂݎ��
			if (Token[TNO].Kind == TK_NAME) {
				if (FuncMode == 1) {
					// �֐��錾
					ClassFuncDecl();
				} else {

					char Name[1024];
					int End,Pointer,Array;

					do {
						End = VarRead(Name,&Pointer,&Array);
						TableAddClass(NK_VAR,Name,Pointer);
						TableAddArray(Array);
					} while (End);

				}
			}

			CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);
		}
		
	} else {
		ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
	}
}

// �N���X���֐��錾
void ClassFuncDecl()
{
	if (Token[TNO].Kind == TK_NAME) {

		//int FuncNameNo = TableAdd(NK_FUNC,Token[TNO].Name,GetCodePos() + 1);
		TNO++;
		CheckToken(TK_LPAREN,ERROR_LPAREN);

		// �p�����[�^�ǂݎ��
		if (Token[TNO].Kind != TK_RPAREN) {

			char Name[1024];
			int End,Pointer,Array;

			do {
				End = VarRead(Name,&Pointer,&Array);
			} while (End);
		}
			
		CheckToken(TK_RPAREN,ERROR_RPAREN);

	} else {
		ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
	}
}

// �ϐ��錾��ǂݎ��B�߂�l���P�̊Ԃ͓ǂݑ����邱�ƁB
int VarRead(char *Name,int *Pointer,int *Array)
{
	if (Token[TNO].Kind == TK_NAME) {
		TNO++;
		// �ϐ��o�^
		if (Token[TNO].Kind == TK_POINTER) { // �|�C���^
			*Pointer = 1;
			*Array = 0;
			strcpy(Name,Token[TNO - 1].Name);
			TNO++;
		} else if (Token[TNO].Kind == TK_LARRAY) { // �z��

			*Pointer = 1;
			NUMLOG(Token[TNO - 1].Name);
			strcpy(Name,Token[TNO - 1].Name);

			TNO++;
			if (Token[TNO].Kind == TK_NUMBER) {
				*Array = Token[TNO].Value;
				TNO++;
			} else {
				*Array = 0;
			}

			CheckToken(TK_RARRAY,ERROR_RARRAY);
		} else { // �ʏ�ϐ�
			*Pointer = 0;
			*Array = 0;
			strcpy(Name,Token[TNO - 1].Name);
		}
	} else {
		ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
	}

	// �J���}�`�F�b�N
	if (Token[TNO].Kind != TK_COMMA) {
		if (Token[TNO].Kind == TK_NAME){
			ErrorSet(ERROR_COMMA,Token[TNO].Line,Token[TNO].Count,NULL,NULL,0,0);
			return 1;
		} else {
			return 0;
		}
	}

	TNO++;

	return 1;
}