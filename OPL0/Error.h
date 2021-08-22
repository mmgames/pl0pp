#ifndef __INCLUDE_ERROR__
#define __INCLUDE_ERROR__

#define ERROR_MAX 100

enum ERRORTYPE {
	ERROR_UNKNOW,		// �����s���G���[
	ERROR_NONAME,		// ���O�ł͂Ȃ�
	ERROR_NUMBER,		// ���l�ł͂Ȃ�
	ERROR_EQUAL,		// �C�R�[�����Ȃ�
	ERROR_SEMICOLON,	// �Z�~�R�������Ȃ�
	ERROR_DO,			// DO���Ȃ�
	ERROR_THEN,			// THEN���Ȃ�
	ERROR_LPAREN,		// �����ʂ��Ȃ�
	ERROR_RPAREN,		// �E���ʂ��Ȃ�
	ERROR_LARRAY,		// [ ���Ȃ�
	ERROR_RARRAY,		// ] ���Ȃ�
	ERROR_COMMA,		// �J���}���Ȃ�
	ERROR_PERIOD,		// . ���Ȃ�
	ERROR_TOKEN,		// ���Ӗ��ȒP��
	ERROR_CONDITION,	// �������Z�q�ł͂Ȃ�
	ERROR_NAME,			// ����`�̖��O
	ERROR_CONST,		// �萔�ւ̑��
	ERROR_CALCADDRESS,	// �v�Z�ł��Ȃ��A�h���X
	ERROR_POINTER,		// �|�C���^�ϐ��ł͂Ȃ�
	ERROR_ARRAY,		// �z��ł͂Ȃ�
	ERROR_ARRAYINPUT,	// �z�񖼂ɂ͑���ł��Ȃ�
	ERROR_TYPE,			// �قȂ�^�ł̑��
};

// �G���[��������
void ErrorInit(void);

// �G���[���e���t�@�C���ɏ����o��
int ErrorWriteFile(const char *File);

// �G���[��������擾
int ErrorGetStr(int No,char *Buf);

// �V�����G���[���L��
int ErrorSet(ERRORTYPE No,int Line,int Count,const char *s1,const char *s2,int i1,int i2);

#endif