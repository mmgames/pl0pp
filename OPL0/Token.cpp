#include <stdio.h>
#include <string.h>
#include "FileSource.h"
#include "Token.h"
#include "LogWindow.h"

static CharKind CharKindTable[256]; // CHARKINDテーブル
static char TokenString[TK_OTHERS][MAXNAME]; // トークン
static char CharRead;

static void TokenCharKindInit();
static void TokenStringInit();
static CharKind TokenGetCharKind(char c);
static void TokenJumpSpace();
static TokenKind TokenGetKind(char Buf[]);

TokenInfo Token[MAXTOKEN];

void NUMLOGTOKEN(int No)
{
	char s[1024];

	if (Token[No].Kind == TK_NAME) {
		strcpy(s,"[名]");
	} else if (Token[No].Kind == TK_NUMBER) {
		strcpy(s,"[数]");
	} else {
		strcpy(s,"[他]");
	}

	sprintf(s,"%s[%2d行%2d字]%2d:%s",s,Token[No].Line,Token[No].Count,No,Token[No].Name);

	TXTLOG(s);
}

int TokenAll(void)
{
	int i;

	TokenInit();

	for (i = 0;i < MAXTOKEN - 1;i++) {

		Token[i] = TokenNext();
		//NUMLOGTOKEN(i);
		if (Token[i].Kind == TK_EOF) break;

	}

	return i;
}

void TokenInit()
{

	CharRead = '\n';	
	TokenCharKindInit();
	TokenStringInit();

	memset(&Token,0,sizeof(Token));

	return;
}

TokenInfo TokenNext()
{
	int i = 0,num = 0;
	char Buf[MAXNAME] = {'\0'};
	TokenKind Kind = TK_OTHERS;
	TokenInfo TokenData;
	
	TokenJumpSpace();	// 空白とコメントの読み飛ばし
	
	// ファイル終端
	if (CharRead == EOF || CharRead == '\0') {
		TokenData.Kind = TK_EOF;
		return TokenData;
	}

	TokenData.Line = FileSourceGetLine();
	TokenData.Count = FileSourceGetCount();

	switch (TokenGetCharKind(CharRead))
	{
	case CK_LETTER: // 名前
		
		do {
			if (i < MAXNAME) Buf[i] = CharRead;
			i++;
			CharRead = FileSourceNextChar();
		} while (TokenGetCharKind(CharRead) == CK_LETTER || TokenGetCharKind(CharRead) == CK_DIGIT);
		
		Buf[i] = '\0';
		
		Kind = TokenGetKind(Buf);
		
		if (Kind != TK_OTHERS) {
			TokenData.Kind = Kind;
		} else {
			TokenData.Kind = TK_NAME;
		}
		
		strcpy(TokenData.Name, Buf);
		
		break;
		
	case CK_DIGIT:
		
		do {
			num = 10 * num + (CharRead - '0');
			i++;
			CharRead = FileSourceNextChar();
		} while (TokenGetCharKind(CharRead) == CK_DIGIT );
		
		TokenData.Kind = TK_NUMBER;
		TokenData.Value = num;
		
		break;
		
	case CK_COLON:
		
		CharRead = FileSourceNextChar();
		
		if (CharRead == '=') {
			CharRead = FileSourceNextChar();
			TokenData.Kind = TK_ASSIGN;
			break;
		} else {
			TokenData.Kind = TK_NULL;
			break;
		}
		
	case CK_LSS:
		
		CharRead = FileSourceNextChar();
		
		if (CharRead == '=') {
			CharRead = FileSourceNextChar();
			TokenData.Kind = TK_LSSEQ;
			break;
		} else if (CharRead == '>') {
			CharRead = FileSourceNextChar();
			TokenData.Kind = TK_NOTEQ;
			break;
		} else {
			TokenData.Kind = TK_LSS;
			break;
		}
		
	case CK_GTR:
		
		CharRead = FileSourceNextChar();
		
		if (CharRead == '=') {
			CharRead = FileSourceNextChar();
			TokenData.Kind = TK_GTREQ;
			break;
		} else {
			TokenData.Kind = TK_GTR;
			break;
		}
		
	default:
		
		Buf[0] = CharRead;
		Buf[1] = '\0';
		
		TokenData.Kind = TokenGetKind(Buf);
		CharRead = FileSourceNextChar();
		
		break;
	}

	// 文字列の設定
	if (TokenData.Kind == TK_NAME) {
	} else if (TokenData.Kind == TK_NUMBER) {
		sprintf(TokenData.Name,"%d",TokenData.Value);
	} else {
		strcpy(TokenData.Name,TokenString[TokenData.Kind]);
	}

	if (TokenData.Kind == TK_STRUCT) TokenData.Kind = TK_CLASS;

	return TokenData;
}

static void TokenJumpSpace()
{

	while (1) {
		
		switch (CharRead) {

		case '#':
			do {
				CharRead = FileSourceNextChar();
			} while (CharRead != '#');
			break;
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			break;
		default:
			return;
		}
		
		CharRead = FileSourceNextChar();
	}
}

static void TokenCharKindInit()
{
	int i;
	
	memset(CharKindTable,CK_OTHERS,sizeof(CharKindTable));
	
	for (i='0'; i<='9'; i++) CharKindTable[i] = CK_DIGIT;
	for (i='A'; i<='Z'; i++) CharKindTable[i] = CK_LETTER;
	for (i='a'; i<='z'; i++) CharKindTable[i] = CK_LETTER;

	CharKindTable['~'] = CK_LETTER;
	CharKindTable['_'] = CK_LETTER;

	CharKindTable['+'] = CK_PLUS;
	CharKindTable['-'] = CK_MINUS;
	CharKindTable['*'] = CK_MULT;
	CharKindTable['/'] = CK_DIV;
	CharKindTable['('] = CK_LPAREN;
	CharKindTable[')'] = CK_RPAREN;
	CharKindTable['='] = CK_EQUAL;
	CharKindTable['<'] = CK_LSS;
	CharKindTable['>'] = CK_GTR;
	CharKindTable[','] = CK_COMMA;
	CharKindTable['.'] = CK_PERIOD;
	CharKindTable[';'] = CK_SEMICOLON;
	CharKindTable[':'] = CK_COLON;
	CharKindTable['['] = CK_LARRAY;
	CharKindTable[']'] = CK_RARRAY;
	CharKindTable['^'] = CK_POINTER;
	CharKindTable['&'] = CK_AND;
}

static void TokenStringInit()
{
	
	strcpy(TokenString[TK_BEGIN],"begin");
	strcpy(TokenString[TK_END],"end");
	strcpy(TokenString[TK_IF],"if");
	strcpy(TokenString[TK_THEN],"then");
	strcpy(TokenString[TK_WHILE],"while");
	strcpy(TokenString[TK_DO],"do");
	strcpy(TokenString[TK_RETURN],"return");
	strcpy(TokenString[TK_FUNC],"function");
	strcpy(TokenString[TK_VAR],"var");
	strcpy(TokenString[TK_CONST],"const");
	strcpy(TokenString[TK_ODD],"odd");
	strcpy(TokenString[TK_WRITE],"write");
	strcpy(TokenString[TK_WRITELN],"writeln");
	strcpy(TokenString[TK_STRUCT],"struct");
	strcpy(TokenString[TK_CLASS],"class");
	strcpy(TokenString[TK_ENDTYPE],"endtype");
	strcpy(TokenString[TK_OBJECT],"object");

	strcpy(TokenString[TK_PLUS],"+");
	strcpy(TokenString[TK_MINUS],"-");
	strcpy(TokenString[TK_MULT],"*");
	strcpy(TokenString[TK_DIV],"/");
	strcpy(TokenString[TK_LPAREN],"(");
	strcpy(TokenString[TK_RPAREN],")");
	strcpy(TokenString[TK_EQUAL],"=");
	strcpy(TokenString[TK_LSS],"<");
	strcpy(TokenString[TK_GTR],">");
	strcpy(TokenString[TK_NOTEQ],"<>");
	strcpy(TokenString[TK_LSSEQ],"<=");
	strcpy(TokenString[TK_GTREQ],">=");
	strcpy(TokenString[TK_COMMA],",");
	strcpy(TokenString[TK_PERIOD],".");
	strcpy(TokenString[TK_SEMICOLON],";");
	strcpy(TokenString[TK_ASSIGN],":=");
	strcpy(TokenString[TK_LARRAY],"[");
	strcpy(TokenString[TK_RARRAY],"]");
	strcpy(TokenString[TK_POINTER],"^");
	strcpy(TokenString[TK_AND],"&");
	
}

static CharKind TokenGetCharKind(char c)
{
	return CharKindTable[c];
}

static TokenKind TokenGetKind(char Buf[])
{
	int i;
	for (i = 0;i < TK_ENDMARK;i++) {
		if (strcmp(Buf,TokenString[i]) == 0) return (TokenKind)i;
	}
	
	return TK_OTHERS;
}
