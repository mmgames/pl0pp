#ifndef __INCLUDE_STATMENT__
#define __INCLUDE_STATMENT__

#include "Error.h"

#define OBJADDRESS 1000 // �����o�[�֐��Ăяo�����ɃI�u�W�F�N�g�̃A�h���X���L������ʒu

extern int TNO; // �g�[�N���̈ʒu
extern int ClassFuncNo;	// �������Ă��郁���o�[�֐�

// ���̍\����͂��s��
int Statement(void);

// ����ׂ��P�ꂪ���݂��邩�`�F�b�N
void CheckToken(TokenKind Kind,ERRORTYPE No);

#endif