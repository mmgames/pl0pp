#include <stdio.h>
#include <string.h>
#include "Table.h"
#include "Token.h"
#include "Error.h"
#include "LogWindow.h"

// ���O��񃊃X�g
static NameInfo NameTable[MAXTABLE];

// �N���X��񃊃X�g
static ClassInfo ClassTable[MAXTABLE];

static int NameCount = -1;		// ��ʃu���b�N�����܂ޖ��O�̌�
static int Addres = -1;			// �O���[�o���̈���ŕϐ����m�ۂ����A�h���X
static int BlockName[MAXLEVEL];	// �e�u���b�N�̖��O�̕ۑ��ʒu
static int Block = -1;			// ���݂̃u���b�N
static int ClassCount = 0;		// ���݂̃N���X�̔ԍ�
static int VarClassNo = 0;	// �錾���̃N���X

// �e�[�u���̏�����
void TableInit()
{
	NameCount = -1;
	Addres = -1;
	Block = -1;

	memset(NameTable,0,sizeof(NameTable));
	memset(BlockName,0,sizeof(BlockName));
}

// �V�����u���b�N�̊J�n
void TableBlockBegin()
{
	if (Block != -1) BlockName[Block] = NameCount;	// ���݂̃u���b�N�̖��O�̈ʒu��ۑ�
	Block++;
	return;
}

// ���݂̃u���b�N�̏I��
void TableBlockEnd()
{
	Block--;
	if (Block != -1) NameCount = BlockName[Block];	// �ȑO�̃u���b�N�̖��O�̈ʒu�𕜌�
}

// �V�������O�̒ǉ�
void TableAddName(NameKind Kind,const char *Name,int Value,int NameCount,int *Addres,NameInfo *NameTable)
{
	NameTable->Kind = Kind;
	strcpy(NameTable->Name, Name);

	switch (Kind) {
		case NK_VAR:
			(*Addres)++;
			NameTable->Address = *Addres;
			NameTable->Pointer = Value;
			NameTable->Class = VarClassNo;
			TXTLOG("[ �ϐ� ]:%3d ��,%3d �A�h���X,%3d �u���b�N,%d �ԃN���X,%s,%s ���O",NameCount,*Addres,Block,VarClassNo,(Value ? "�|�C���^" : "�ʏ�ϐ�"),Name);
			if (VarClassNo != 0) (*Addres) += ClassTable[VarClassNo].Addres;
			break;
		case NK_CONST:
			NameTable->Value = Value;
			TXTLOG("[ �萔 ]:%3d ��,%3d �u���b�N,%3d �l,%s ���O",NameCount,Block,Value,Name);
			break;
		case NK_FUNC:
			NameTable->Address = Value;
			TXTLOG("[ �֐� ]:%3d ��,%3d �A�h���X,%s ���O",NameCount,Value,Name);
			break;
		case NK_CLASS:
			ClassCount++;
			strcpy(ClassTable[ClassCount].Name,Name);
			ClassTable[ClassCount].NameCount = -1;
			ClassTable[ClassCount].Addres = -1;
			ClassTable[ClassCount].Func = 0;
			ClassTable[ClassCount].Var = 0;
			TXTLOG("[�N���X]:%3d ��,%d �ԃN���X,%s ���O",NameCount,ClassCount,Name);
			break;
	}
}

// �V�������O�̒ǉ�
int TableAdd(NameKind Kind,const char *Name,int Value)
{
	NameCount++;
	TableAddName(Kind,Name,Value,NameCount,&Addres,&NameTable[NameCount]);
	return NameCount;
}

// �N���X�ւ̐V�������O�̒ǉ�
int TableAddClass(NameKind Kind,const char *Name,int Value)
{
	ClassTable[ClassCount].NameCount++;
	TableAddName(Kind,Name,Value,
		ClassTable[ClassCount].NameCount,
		&ClassTable[ClassCount].Addres,
		&ClassTable[ClassCount].NameTable[ClassTable[ClassCount].NameCount]);
	return ClassTable[ClassCount].NameCount;
}

// ���ꂩ��錾����N���X�̌^
int TableAddVarClass(const char *Name)
{
	if (Name != NULL) {
		VarClassNo = TableGetClassNo(Name);
	} else {
		VarClassNo = 0;
	}

	return VarClassNo;
}

// ���O�ɓo�^�����ϐ���z��ɐݒ�
void TableAddArray(int Count)
{
	if (Count == 0) return;
	Addres += Count; // �v�f�������P�����m�ۂ��Ă���
	NameTable[NameCount].Array = Count;
	TXTLOG("��L�ϐ���z��Ɏw�� %3d �v�f��",Count);
}

// �֐��̃p�����[�^��ݒ�
void TableFuncParAddress(int FuncNameCount,int VarNameCount)
{
	NameTable[FuncNameCount].ParAddress[NameTable[FuncNameCount].Pars] = NameTable[VarNameCount].Address;
	TXTLOG("[ ���� ]:%s �֐�,%3d ��,%s �ϐ�,%3d �A�h���X",NameTable[FuncNameCount].Name,NameTable[FuncNameCount].Pars,NameTable[VarNameCount].Name,NameTable[VarNameCount].Address);
	NameTable[FuncNameCount].Pars++;
}

// �֐��̃p�����[�^�̃A�h���X���擾
int TableGetFuncParAddress(int FuncNameCount,int ParCount)
{
	return NameTable[FuncNameCount].ParAddress[ParCount];
}

// �����̖��O�̔ԍ����擾
int TableGetNameNo(const char *Name)
{
	int i;
	
	// ��납��T�����ƂŃu���b�N�̗D�揇�ʂɏ]��
	for (i = NameCount;i >= 0;i--) {
		if (strcmp(Name, NameTable[i].Name) == 0) return i;
	}
	
	return -1;
}

// �����̖��O�̏����擾
NameInfo TableGetNameInfo(const char *Name)
{
	int No = TableGetNameNo(Name);

	if (No == -1) {
		NameInfo Name = {NK_NON};
		return Name;
	}

	return NameTable[No];
}

// �����̃N���X�̔ԍ����擾
int TableGetClassNo(const char *Name)
{
	int i;
	
	// ��납��T�����ƂŃu���b�N�̗D�揇�ʂɏ]��
	for (i = 1;i <= ClassCount;i++) {
		if (strcmp(Name, ClassTable[i].Name) == 0) return i;
	}
	
	return 0;
}

// �����̃N���X�̏����擾
ClassInfo TableGetClassInfo(int ClassNo)
{
	return ClassTable[ClassNo];
}

// �N���X��������̖��O�̔ԍ����擾
int TableGetClassNameNo(int ClassNo,const char *Name)
{
	int i;
	
	// ��납��T�����ƂŃu���b�N�̗D�揇�ʂɏ]��
	for (i = NameCount;i >= 0;i--) {
		if (strcmp(Name, ClassTable[ClassNo].NameTable[i].Name) == 0) return i;
	}
	
	return -1;
}

// �N���X��������̖��O�̏����擾
NameInfo TableGetClassNameInfo(int ClassNo,const char *Name)
{
	int No = TableGetClassNameNo(ClassNo,Name);

	if (No == -1) {
		NameInfo Name = {NK_NON};
		return Name;
	}

	return ClassTable[ClassNo].NameTable[No];
}