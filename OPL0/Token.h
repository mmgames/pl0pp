#ifndef __INCLUDE_TOKEN__
#define __INCLUDE_TOKEN__

#include <stdio.h>
#include <string.h>

#define MAXNAME 256
#define MAXTOKEN 1024

enum CharKind { // �����̎��
	CK_OTHERS,		// ����`
	CK_DIGIT,		// ����
	CK_LETTER,		// �A���t�@�x�b�g
	CK_PLUS,		// +
	CK_MINUS,		// -
	CK_MULT,		// *
	CK_DIV,			// /
	CK_LPAREN,		// (
	CK_RPAREN,		// )
	CK_EQUAL,		// =
	CK_LSS,			// <
	CK_GTR,			// >
	CK_COMMA,		// ,
	CK_PERIOD,		// .
	CK_SEMICOLON,	// ;
	CK_COLON,		// :
	CK_LARRAY,		// [
	CK_RARRAY,		// ]
	CK_POINTER,		// ^
	CK_AND,			// &
};

enum TokenKind { // �g�[�N���̎��
	// �\���
	TK_BEGIN,
	TK_END,
	TK_IF,
	TK_THEN,
	TK_WHILE,
	TK_DO,
	TK_RETURN,
	TK_FUNC, 
	TK_VAR,
	TK_CONST,
	TK_ODD,
	TK_WRITE,
	TK_WRITELN,
	TK_STRUCT,
	TK_CLASS,
	TK_ENDTYPE,
	TK_OBJECT,
	TK_ENDKEYWORD,
	// �L��
	TK_PLUS,	// +
	TK_MINUS,	// -
	TK_MULT,	// *
	TK_DIV,		// /
	TK_LPAREN,	// (
	TK_RPAREN,	// )
	TK_EQUAL,	// =
	TK_LSS,		// <
	TK_GTR,		// >
	TK_NOTEQ,	// <>
	TK_LSSEQ,	// <=
	TK_GTREQ, 	// >=
	TK_COMMA,	// ,
	TK_PERIOD,	// .
	TK_SEMICOLON,	// ;
	TK_ASSIGN,	// :=
	TK_LARRAY,	// [
	TK_RARRAY,	// ]
	TK_POINTER,	// ^
	TK_AND,		// &
	TK_ENDMARK,
	// ���[�U�[��`
	TK_NAME,	// ���O
	TK_NUMBER,	// ����
	TK_NULL,
	// ���̑�
	TK_EOF,		// EOF
	TK_OTHERS,	// ����`
};

// �g�[�N�����
typedef struct {
	TokenKind Kind;
	char Name[MAXNAME];
	int Value;
	int Line; // �s
	int Count; // �����ʒu
} TokenInfo;

// �g�[�N���f�[�^
extern TokenInfo Token[MAXTOKEN];

// �g�[�N���ǂݍ��ݑO�̏�����
void TokenInit();

// ���̃g�[�N����ǂݍ���
TokenInfo TokenNext();

// �����񒆂̑S�Ẵg�[�N����ǂݍ���
int TokenAll(void); 

#endif
