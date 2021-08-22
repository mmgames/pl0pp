#ifndef __INCLUDE_TOKEN__
#define __INCLUDE_TOKEN__

#include <stdio.h>
#include <string.h>

#define MAXNAME 256
#define MAXTOKEN 1024

enum CharKind { // 文字の種類
	CK_OTHERS,		// 未定義
	CK_DIGIT,		// 数字
	CK_LETTER,		// アルファベット
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

enum TokenKind { // トークンの種類
	// 予約語
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
	// 記号
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
	// ユーザー定義
	TK_NAME,	// 名前
	TK_NUMBER,	// 数字
	TK_NULL,
	// その他
	TK_EOF,		// EOF
	TK_OTHERS,	// 未定義
};

// トークン情報
typedef struct {
	TokenKind Kind;
	char Name[MAXNAME];
	int Value;
	int Line; // 行
	int Count; // 文字位置
} TokenInfo;

// トークンデータ
extern TokenInfo Token[MAXTOKEN];

// トークン読み込み前の初期化
void TokenInit();

// 次のトークンを読み込む
TokenInfo TokenNext();

// 文字列中の全てのトークンを読み込む
int TokenAll(void); 

#endif
