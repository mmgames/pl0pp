#include <stdio.h>
#include <string.h>
#include "Token.h"
#include "Statement.h"
#include "Grammar.h"
#include "Table.h"
#include "Code.h"
#include "LogWindow.h"

int TNO; // トークンの位置
int ClassFuncNo;	// 処理しているメンバー関数

static int VarRead(NameInfo Name,int *Kind); // 変数の読み込み
static int ObjectRead(NameInfo *Name); // オブジェクトの読み込み
static int VarKind(NameInfo *Name); // 変数の種類を取得し、アドレス計算を行う
static void VarInt(NameInfo Name); // 変数への代入
static void VarOut(NameInfo Name); // 変数の値取得

static void Block(void); // ブロックの解析
static void Name(void); // 名前の解析
static void While(void); // While文の解析
static void Write(void); // Write文の解析
static void Writeln(void); // Writeln文の解析
static void If(void); // If文の解析
static void Return(void); // Return文の解析

static void ObjectInput(NameInfo Name); // オブジェクト同士の代入

static void Expression(void); // 式の解析(足し算と引き算)
static void Term(void); // 式の解析(掛け算と割り算)
static void Factor(void); // 式の解析(因子(数値))
static void Condition(void); // 条件式の解析
static void FuncRun(char *ObjectName); // 関数呼び出し

// あるべき単語が存在するかチェック
void CheckToken(TokenKind Kind,ERRORTYPE No)
{
	if (Token[TNO].Kind != Kind) {
		ErrorSet(No,Token[TNO].Line,Token[TNO].Count,NULL,NULL,0,0);
	} else {
		TNO++;
	}
}

// 文の構文解析を行う
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

// ブロックの解析
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

// 変数の読み込み
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
		if (Name.Array > 0) { // 配列
			CodeAdd(VC_SAVE,STOCK,Name.Address);
		} else { // ポインタ
			CodeAdd(VC_COPY,Name.Address,STOCK);
		}
		CodeAdd(VC_CALC,OR_ADD,0);
		CheckToken(TK_RARRAY,ERROR_RARRAY);
	}

	return 0;
}

// オブジェクトの読み込み
int ObjectRead(NameInfo *Name)
{
	if (Name->Class == 0) return 0;

	// オブジェクト同士の代入
	if (Token[TNO].Kind == TK_EQUAL) {	
		ObjectInput(*Name);
		return 1;
	}

	CheckToken(TK_PERIOD,ERROR_PERIOD);

	// オブジェクトからメンバー関数呼び出し
	if (Token[TNO + 1].Kind == TK_LPAREN) {	
		FuncRun(Name->Name);
		return 1;
	}

	// オブジェクトのメンバー変数
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

// 変数の種類を取得し、アドレス計算を行う
int VarKind(NameInfo *Name)
{
	int Kind = 0;
	int ObjVar = 0;

	TNO++;

	// オブジェクトの判定
	if (ObjectRead(Name) == 1) return -1;

	// メンバーの判定
	if (ClassFuncNo != 0) {
		NameInfo NameC = TableGetClassNameInfo(ClassFuncNo,Name->Name);
		if (NameC.Kind != NK_NON) {
			ObjVar = 1;
			*Name = NameC;
		}
	}

	// 変数読み取り
	VarRead(*Name,&Kind);

	// メンバー変数
	if (ObjVar == 1 && Kind == 0) {
		CodeAdd(VC_SAVE,STOCK,Name->Address);
		CodeAdd(VC_COPY,OBJADDRESS,STOCK);
		CodeAdd(VC_CALC,OR_ADD,0);
		if (Kind == 0) Kind = 2;
	}

	return Kind;
}

// 変数への代入
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

// 変数の値取得
void VarOut(NameInfo Name)
{
	int Kind = VarKind(&Name);
	if (Kind == -1) return;

	switch (Kind) {
	case 0:
		if (Name.Array > 0) { // 配列名
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

// 文の先頭が名前だった場合
void Name(void)
{
	int Kind = 0;

	NameInfo Name = TableGetNameInfo(Token[TNO].Name);

	// クラスのメンバー処理中
	if (ClassFuncNo != 0) {

		// メンバー関数判定
		char FuncName[1024];
		ClassInfo ClassData = TableGetClassInfo(ClassFuncNo);
		sprintf(FuncName,"%s_%s",ClassData.Name,Token[TNO].Name);
		if (TableGetNameNo(FuncName) != -1) {
			Name = TableGetNameInfo(FuncName);
			strcpy(Token[TNO].Name,FuncName);
		}

		// メンバー変数判定
		NameInfo NameC = TableGetClassNameInfo(ClassFuncNo,Token[TNO].Name);
		if (NameC.Kind != NK_NON) Name = NameC;

	}

	switch (Name.Kind)
	{
	case NK_FUNC: // 関数
		FuncRun(NULL);
		return;
	case NK_VAR: // 変数名
		VarInt(Name);
		break;
	case NK_CONST: // 定数名
		ErrorSet(ERROR_CONST,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		break;
	default:
		ErrorSet(ERROR_NAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		break;
	}
}

// オブジェクト同士の代入
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

// 関数呼び出し
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

			// パラメータ読み取り
			int ParCount = 0;
			while (1) {

				Expression();
				int VarAddress = TableGetFuncParAddress(FuncNameNo,ParCount); // 引数のアドレス取得
				CodeAdd(VC_COPY,STOCK,VarAddress); // 引数に値を代入

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

// While文の解析
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

// Write文の解析
void Write()
{
	TNO++;
	Expression();
	CodeAdd(VC_PRINT,0,0);
}

// Writeln文の解析
void Writeln(void)
{
	TNO++;
	CodeAdd(VC_NEWLINE,0,0);
	return;
}

// If文の解析
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

// Return文の解析
void Return(void)
{
	TNO++;
	Expression();
	CodeAdd(VC_RETURN,0,0);
}

// 式の解析(足し算と引き算)
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

// 式の解析(掛け算と割り算)
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

// 式の解析(因子(数値))
void Factor(void)
{
	NameInfo Name;

	switch (Token[TNO].Kind)
	{
	case TK_NAME:

		Name = TableGetNameInfo(Token[TNO].Name);

		// メンバーの判定
		if (ClassFuncNo != 0) {
			NameInfo NameC = TableGetClassNameInfo(ClassFuncNo,Token[TNO].Name);
			if (NameC.Kind != NK_NON) Name = NameC;
		}

		switch (Name.Kind)
		{
		case NK_VAR: // 変数
			VarOut(Name);
			break;
		case NK_CONST: // 定数
			CodeAdd(VC_SAVE,STOCK,Name.Value);
			TNO++;
			break;
		case NK_FUNC: // 関数呼び出し
			FuncRun(NULL);
			break;
		}

		break;
	case TK_NUMBER: // 定数
		CodeAdd(VC_SAVE,STOCK,Token[TNO].Value);
		TNO++;
		break;
	case TK_AND: // アドレス計算
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

// 条件式の解析
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