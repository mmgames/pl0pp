#include <stdio.h>
#include <string.h>
#include "Token.h"
#include "Statement.h"
#include "Grammar.h"
#include "Table.h"
#include "Code.h"
#include "LogWindow.h"

int TNO; // �g�[�N���̈ʒu
int ClassFuncNo;	// �������Ă��郁���o�[�֐�

static int VarRead(NameInfo Name,int *Kind); // �ϐ��̓ǂݍ���
static int ObjectRead(NameInfo *Name); // �I�u�W�F�N�g�̓ǂݍ���
static int VarKind(NameInfo *Name); // �ϐ��̎�ނ��擾���A�A�h���X�v�Z���s��
static void VarInt(NameInfo Name); // �ϐ��ւ̑��
static void VarOut(NameInfo Name); // �ϐ��̒l�擾

static void Block(void); // �u���b�N�̉��
static void Name(void); // ���O�̉��
static void While(void); // While���̉��
static void Write(void); // Write���̉��
static void Writeln(void); // Writeln���̉��
static void If(void); // If���̉��
static void Return(void); // Return���̉��

static void ObjectInput(NameInfo Name); // �I�u�W�F�N�g���m�̑��

static void Expression(void); // ���̉��(�����Z�ƈ����Z)
static void Term(void); // ���̉��(�|���Z�Ɗ���Z)
static void Factor(void); // ���̉��(���q(���l))
static void Condition(void); // �������̉��
static void FuncRun(char *ObjectName); // �֐��Ăяo��

// ����ׂ��P�ꂪ���݂��邩�`�F�b�N
void CheckToken(TokenKind Kind,ERRORTYPE No)
{
	if (Token[TNO].Kind != Kind) {
		ErrorSet(No,Token[TNO].Line,Token[TNO].Count,NULL,NULL,0,0);
	} else {
		TNO++;
	}
}

// ���̍\����͂��s��
int Statement(void)
{
	while(1) {
		switch (Token[TNO].Kind) {

		case TK_BEGIN: Block(); TNO++; return 0;
		case TK_END: return 0;
		case TK_SEMICOLON: return 0;

		case TK_NAME: Name(); return 0;
		case TK_WHILE: While(); return 0;
		case TK_IF: If(); return 0;
		case TK_RETURN: Return(); return 0;
		case TK_WRITE: Write(); return 0;
		case TK_WRITELN: Writeln(); return 0;

		default: ErrorSet(ERROR_TOKEN,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0); TNO++; continue;
		}
	}

	return 0;
}

// �u���b�N�̉��
void Block(void)
{
	TNO++;
	while(1)
	{
		Statement();
		while(1)
		{
			if (Token[TNO].Kind == TK_SEMICOLON) {
				TNO++;
				break;
			}

			if (Token[TNO].Kind == TK_END) return;

			ErrorSet(ERROR_SEMICOLON,Token[TNO].Line,Token[TNO].Count,NULL,NULL,0,0);

			TNO++;
		}
	}
}

// �ϐ��̓ǂݍ���
int VarRead(NameInfo Name,int *Kind)
{
	*Kind = 0;

	if (Token[TNO].Kind == TK_POINTER) {
		if (Name.Pointer == 0) ErrorSet(ERROR_POINTER,Token[TNO - 1].Line,Token[TNO - 1].Count,Token[TNO - 1].Name,NULL,0,0);
		*Kind = 1;
		TNO++;
	} else if (Token[TNO].Kind == TK_LARRAY) {
		if (Name.Pointer == 0) ErrorSet(ERROR_ARRAY,Token[TNO - 1].Line,Token[TNO - 1].Count,Token[TNO - 1].Name,NULL,0,0);
		*Kind = 2;
		TNO++;
		Expression();
		if (Name.Array > 0) { // �z��
			CodeAdd(VC_SAVE,STOCK,Name.Address);
		} else { // �|�C���^
			CodeAdd(VC_COPY,Name.Address,STOCK);
		}
		CodeAdd(VC_CALC,OR_ADD,0);
		CheckToken(TK_RARRAY,ERROR_RARRAY);
	}

	return 0;
}

// �I�u�W�F�N�g�̓ǂݍ���
int ObjectRead(NameInfo *Name)
{
	if (Name->Class == 0) return 0;

	// �I�u�W�F�N�g���m�̑��
	if (Token[TNO].Kind == TK_EQUAL) {	
		ObjectInput(*Name);
		return 1;
	}

	CheckToken(TK_PERIOD,ERROR_PERIOD);

	// �I�u�W�F�N�g���烁���o�[�֐��Ăяo��
	if (Token[TNO + 1].Kind == TK_LPAREN) {	
		FuncRun(Name->Name);
		return 1;
	}

	// �I�u�W�F�N�g�̃����o�[�ϐ�
	if (Token[TNO].Kind == TK_NAME) {
		NameInfo NameC = TableGetClassNameInfo(Name->Class,Token[TNO].Name);
		NameC.Address += Name->Address;
		*Name = NameC;
		TNO++;
	} else {
		ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
	}

	return 0;
}

// �ϐ��̎�ނ��擾���A�A�h���X�v�Z���s��
int VarKind(NameInfo *Name)
{
	int Kind = 0;
	int ObjVar = 0;

	TNO++;

	// �I�u�W�F�N�g�̔���
	if (ObjectRead(Name) == 1) return -1;

	// �����o�[�̔���
	if (ClassFuncNo != 0) {
		NameInfo NameC = TableGetClassNameInfo(ClassFuncNo,Name->Name);
		if (NameC.Kind != NK_NON) {
			ObjVar = 1;
			*Name = NameC;
		}
	}

	// �ϐ��ǂݎ��
	VarRead(*Name,&Kind);

	// �����o�[�ϐ�
	if (ObjVar == 1 && Kind == 0) {
		CodeAdd(VC_SAVE,STOCK,Name->Address);
		CodeAdd(VC_COPY,OBJADDRESS,STOCK);
		CodeAdd(VC_CALC,OR_ADD,0);
		if (Kind == 0) Kind = 2;
	}

	return Kind;
}

// �ϐ��ւ̑��
void VarInt(NameInfo Name)
{
	int Kind = VarKind(&Name);
	if (Kind == -1) return;

	if (Kind == 0 && Name.Array > 0) ErrorSet(ERROR_ARRAYINPUT,Token[TNO - 1].Line,Token[TNO - 1].Count,Token[TNO - 1].Name,NULL,0,0);

	CheckToken(TK_EQUAL,ERROR_EQUAL);
	Expression();

	switch (Kind) {
	case 0:
		CodeAdd(VC_COPY,STOCK,Name.Address);
		break;
	case 1:
		CodeAdd(VC_PINT,Name.Address,0);
		break;
	case 2:
		CodeAdd(VC_PINT,STOCK,0);
		break;
	}
}

// �ϐ��̒l�擾
void VarOut(NameInfo Name)
{
	int Kind = VarKind(&Name);
	if (Kind == -1) return;

	switch (Kind) {
	case 0:
		if (Name.Array > 0) { // �z��
			CodeAdd(VC_SAVE,STOCK,Name.Address);
		} else {
			CodeAdd(VC_COPY,Name.Address,STOCK);
		}
		break;
	case 1:
		CodeAdd(VC_POUT,Name.Address,0);
		break;
	case 2:
		CodeAdd(VC_POUT,STOCK,0);
		break;
	}
}

// ���̐擪�����O�������ꍇ
void Name(void)
{
	int Kind = 0;

	NameInfo Name = TableGetNameInfo(Token[TNO].Name);

	// �N���X�̃����o�[������
	if (ClassFuncNo != 0) {

		// �����o�[�֐�����
		char FuncName[1024];
		ClassInfo ClassData = TableGetClassInfo(ClassFuncNo);
		sprintf(FuncName,"%s_%s",ClassData.Name,Token[TNO].Name);
		if (TableGetNameNo(FuncName) != -1) {
			Name = TableGetNameInfo(FuncName);
			strcpy(Token[TNO].Name,FuncName);
		}

		// �����o�[�ϐ�����
		NameInfo NameC = TableGetClassNameInfo(ClassFuncNo,Token[TNO].Name);
		if (NameC.Kind != NK_NON) Name = NameC;

	}

	switch (Name.Kind)
	{
	case NK_FUNC: // �֐�
		FuncRun(NULL);
		return;
	case NK_VAR: // �ϐ���
		VarInt(Name);
		break;
	case NK_CONST: // �萔��
		ErrorSet(ERROR_CONST,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		break;
	default:
		ErrorSet(ERROR_NAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		break;
	}
}

// �I�u�W�F�N�g���m�̑��
static void ObjectInput(NameInfo Name)
{
	ClassInfo ClassData = TableGetClassInfo(Name.Class);

	TNO++;
	if (Token[TNO].Kind == TK_NAME) {
		NameInfo Name2 = TableGetNameInfo(Token[TNO].Name);
		if (Name.Class == Name2.Class) {
			int i;
			for (i = 0;i <= ClassData.Addres;i++) {
				CodeAdd(VC_COPY,Name2.Address + i,Name.Address + i);
			}
			TNO++;
		} else {
			ErrorSet(ERROR_TYPE,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		}
	} else {
		ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
	}
}

// �֐��Ăяo��
void FuncRun(char *ObjectName)
{
	char FuncName[1024];
	NameInfo ObjName;
	ClassInfo ClassData;

	if (ObjectName == NULL) {
		strcpy(FuncName,Token[TNO].Name);
	} else {
		ObjName = TableGetNameInfo(ObjectName);
		ClassData = TableGetClassInfo(ObjName.Class);
		sprintf(FuncName,"%s_%s",ClassData.Name,Token[TNO].Name);
		CodeAdd(VC_SAVE,OBJADDRESS,ObjName.Address);
	}

	int FuncNameNo = TableGetNameNo(FuncName);
	NameInfo Name = TableGetNameInfo(FuncName);

	TNO++;
	if (Token[TNO].Kind == TK_LPAREN) {

		TNO++;
		if (Token[TNO].Kind != TK_RPAREN) {

			// �p�����[�^�ǂݎ��
			int ParCount = 0;
			while (1) {

				Expression();
				int VarAddress = TableGetFuncParAddress(FuncNameNo,ParCount); // �����̃A�h���X�擾
				CodeAdd(VC_COPY,STOCK,VarAddress); // �����ɒl����

				ParCount++;

				if (Token[TNO].Kind == TK_COMMA) {
					TNO++;
					continue;
				}
				CheckToken(TK_RPAREN,ERROR_RPAREN);
				break;
			}

		} else {
			TNO++;
		}

		CodeAdd(VC_CALL,Name.Address,0);

	} else {
		ErrorSet(ERROR_LPAREN,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		ErrorSet(ERROR_RPAREN,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
	}
}

// While���̉��
void While(void)
{
	TNO++;
	int WhileAddress = GetCodePos() + 1;

	Condition();
	CheckToken(TK_DO,ERROR_DO);

	CodeAdd(VC_JUMP0,0,0);
	int JumpAddress = GetCodePos();

	Statement();
	CodeAdd(VC_JUMP,WhileAddress,0);
	CodeRevision(JumpAddress,GetCodePos() + 1,0);
}

// Write���̉��
void Write()
{
	TNO++;
	Expression();
	CodeAdd(VC_PRINT,0,0);
}

// Writeln���̉��
void Writeln(void)
{
	TNO++;
	CodeAdd(VC_NEWLINE,0,0);
	return;
}

// If���̉��
void If(void)
{
	TNO++;
	Condition();
	CheckToken(TK_THEN,ERROR_THEN);
	CodeAdd(VC_JUMP0,0,0);
	int IfAddress = GetCodePos();
	Statement();
	CodeRevision(IfAddress,GetCodePos() + 1,0);
}

// Return���̉��
void Return(void)
{
	TNO++;
	Expression();
	CodeAdd(VC_RETURN,0,0);
}

// ���̉��(�����Z�ƈ����Z)
void Expression(void)
{	
	TokenKind Kind = Token[TNO].Kind;

	if (Kind == TK_PLUS || Kind == TK_MINUS) {
		TNO++;
		Term();
		if (Kind == TK_MINUS) CodeAdd(VC_CALC,OR_NEG,0);
	} else {
		Term();
	}

	Kind = Token[TNO].Kind;

	while (Kind == TK_PLUS || Kind == TK_MINUS) {
		TNO++;
		Term();
		if (Kind == TK_MINUS) CodeAdd(VC_CALC,OR_SUB,0); else CodeAdd(VC_CALC,OR_ADD,0);
		Kind = Token[TNO].Kind;
	}

}

// ���̉��(�|���Z�Ɗ���Z)
void Term(void)
{
	TokenKind Kind;
	Factor();
	Kind = Token[TNO].Kind;
	while (Kind == TK_MULT || Kind == TK_DIV) {
		TNO++;
		Factor();
		if (Kind == TK_MULT) CodeAdd(VC_CALC,OR_MUL,0); else CodeAdd(VC_CALC,OR_DIV,0);
		Kind = Token[TNO].Kind;
	}
}

// ���̉��(���q(���l))
void Factor(void)
{
	NameInfo Name;

	switch (Token[TNO].Kind)
	{
	case TK_NAME:

		Name = TableGetNameInfo(Token[TNO].Name);

		// �����o�[�̔���
		if (ClassFuncNo != 0) {
			NameInfo NameC = TableGetClassNameInfo(ClassFuncNo,Token[TNO].Name);
			if (NameC.Kind != NK_NON) Name = NameC;
		}

		switch (Name.Kind)
		{
		case NK_VAR: // �ϐ�
			VarOut(Name);
			break;
		case NK_CONST: // �萔
			CodeAdd(VC_SAVE,STOCK,Name.Value);
			TNO++;
			break;
		case NK_FUNC: // �֐��Ăяo��
			FuncRun(NULL);
			break;
		}

		break;
	case TK_NUMBER: // �萔
		CodeAdd(VC_SAVE,STOCK,Token[TNO].Value);
		TNO++;
		break;
	case TK_AND: // �A�h���X�v�Z
		TNO++;
		Name = TableGetNameInfo(Token[TNO].Name);
		if (Name.Kind == NK_VAR) {
			CodeAdd(VC_SAVE,STOCK,Name.Address);
			TNO++;
		} else {
			ErrorSet(ERROR_CALCADDRESS,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		}
		break;
	default:
		ErrorSet(ERROR_NAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		break;
	}
}

// �������̉��
void Condition(void)
{
	TokenKind Kind;

	if (Token[TNO].Kind == TK_ODD) {

		TNO++;
		Expression();
		CodeAdd(VC_CALC,OR_ODD,0);

	} else {

		Expression();
		Kind = Token[TNO].Kind;
		TNO++;
		Expression();

		switch(Kind)
		{
		case TK_EQUAL:	CodeAdd(VC_CALC,OR_EQ,0); break;
		case TK_LSS:	CodeAdd(VC_CALC,OR_LS,0); break;
		case TK_GTR:	CodeAdd(VC_CALC,OR_GR,0); break;
		case TK_NOTEQ:	CodeAdd(VC_CALC,OR_NEQ,0); break;
		case TK_LSSEQ:	CodeAdd(VC_CALC,OR_LSEQ,0); break;
		case TK_GTREQ:	CodeAdd(VC_CALC,OR_GREQ,0); break;
		default: ErrorSet(ERROR_CONDITION,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0); break;
		}
	}
}