#ifndef _INCLUDE_TABLE_
#define _INCLUDE_TABLE_

#include "Token.h"

#define MAXLEVEL 20
#define MAXTABLE 256
#define MAXCLASSVAR 30

// ���O�̎��
enum NameKind {
	NK_NON,			// �s���Ȏ��
	NK_VAR,			// �ϐ�
	NK_FUNC,		// �֐�
	NK_CONST,		// �萔
	NK_CLASS,		// �N���X�^
	NK_OBJECT,		// �I�u�W�F�N�g
};

// ���O���
typedef struct {
	NameKind Kind;		// ���
	char Name[MAXNAME]; // ���O
	int Pointer;		// �|�C���^���ۂ�
	int Array;			// �z��̗v�f���B�O�Ȃ�P��ϐ��B
	int Value;			// �l
	int Address;		// �A�h���X
	int Pars;			// �p�����[�^��
	int ParAddress[20];	// �e�p�����[�^�̃A�h���X
	int Class;			// �N���X�ԍ�
} NameInfo;

// �N���X���
typedef struct {
	char Name[MAXNAME];		// �N���X��
	int Var;				// �v�f��
	int Func;				// �����o�[�֐���
	NameInfo NameTable[MAXCLASSVAR];	// �e�v�f�̏��
	int NameCount;			// ���O�̌�
	int Addres;				// �\���̓��ł̑��΃A�h���X
} ClassInfo;

// �e�[�u���̏�����
void TableInit();

// �V�����u���b�N�̊J�n
void TableBlockBegin();

// ���݂̃u���b�N�̏I��
void TableBlockEnd();

// �V�������O�̒ǉ�
int TableAdd(NameKind Kind,const char *Name,int Value);

// �N���X�ւ̐V�������O�̒ǉ�
int TableAddClass(NameKind Kind,const char *Name,int Value);

// ���O�ɓo�^�����ϐ���z��ɐݒ�
void TableAddArray(int Count);

// ���ꂩ��錾����N���X�̌^
int TableAddVarClass(const char *Name);

// �֐��̃p�����[�^��ݒ�
void TableFuncParAddress(int FuncNameCount,int VarNameCount);

// �֐��̃p�����[�^�̃A�h���X���擾
int TableGetFuncParAddress(int FuncNameCount,int ParCount);

// �����̖��O�̔ԍ����擾
int TableGetNameNo(const char *Name);

// �����̖��O�̏����擾
NameInfo TableGetNameInfo(const char *Name);

// �����̃N���X�̔ԍ����擾
int TableGetClassNo(const char *Name);

// �����̃N���X�̏����擾
ClassInfo TableGetClassInfo(int ClassNo);

// �N���X��������̖��O�̔ԍ����擾
int TableGetClassNameNo(int ClassNo,const char *Name);

// �N���X��������̖��O�̏����擾
NameInfo TableGetClassNameInfo(int ClassNo,const char *Name);

#endif