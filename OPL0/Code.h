#ifndef _INCLUDE_MIDDLECODE_
#define _INCLUDE_MIDDLECODE_

#define MIDDLECODE_MAX 1024
#define STOCK -1	//�X�^�b�N�̒�����Ӗ�����

enum VirtualCode {	// ���z����
	VC_SAVE,	// �萔�l���L������F�L����A���l
	VC_COPY,	// ����������R�s�[����F�R�s�[���A�R�s�[��
	VC_POUT,		// �|�C���^����l���X�^�b�N�ɃR�s�[����F�|�C���^
	VC_PINT,		// �X�^�b�N����|�C���^�ɒl���R�s�[����F�|�C���^
	VC_CALC,	// �X�^�b�N�l���v�Z����F���Z�q
	VC_PRINT,	// �X�^�b�N�l��\������
	VC_NEWLINE,	// ���s����
	VC_CALL,	// �֐��Ăяo���F�W�����v��
	VC_RETURN,	// �֐��Ăяo������̕��A
	VC_JUMP,	// �������W�����v�F�W�����v��
	VC_JUMP0,	// �X�^�b�N�l���O�̎��W�����v�F�W�����v��
	VC_NON,		// �������Ȃ�
	VC_END,		// �v���O�����̋����I��
};

enum Operation {	// ���Z�q�ꗗ

	OR_NEG,		// �������]
	OR_ADD,		// ���Z
	OR_SUB,		// ���Z
	OR_MUL,		// ��Z
	OR_DIV,		// ���Z

	OR_ODD,		// ��̎��P
	OR_EQ,		// ���������P
	OR_LS,		// �������������P
	OR_GR,		// �E�����������P
	OR_NEQ,		// �قȂ�Ƃ��P
	OR_LSEQ,	// �����ȉ��̎��P
	OR_GREQ,	// �E���ȉ��̎��P
};

typedef struct {	// ���ߏ��
	VirtualCode VCode;	// ����
	int DataA;			// ��P�p�����[�^
	int DataB;			// ��Q�p�����[�^
} CodeInfo;

// ���߃��X�g
extern CodeInfo Code[MIDDLECODE_MAX];

// ������ 
void CodeInit(void);

// �V�������߂�ǉ�
void CodeAdd(VirtualCode Code,int DataA,int DataB);

// �����̖��߂��C������(�o�b�N�p�b�`)
void CodeRevision(int No,int DataA,int DataB);

// ���O�ɒǉ��������߂̃A�h���X
int GetCodePos(void);

// ���߃��X�g��\��
void CodeShow();

// ���߂����s
void Interpreter(char *Result,int StackView = 0);

#endif