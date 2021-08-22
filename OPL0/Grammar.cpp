#include <stdio.h>
#include <string.h>
#include "Token.h"
#include "Grammar.h"
#include "Statement.h"
#include "Table.h"
#include "Code.h"
#include "Error.h"
#include "LogWindow.h"

static int FuncBlock = 0;			// 関数ブロック
static int FuncBlockCode[30];		// 宣言中に作成したコードサイズ
static int FuncBlockDest[30];		// ブロック内のデストラクタオブジェクトの数
static char FuncBlockDestName[30][30][MAXNAME];		// ブロック内のデストラクタのオブジェクト名

static void Block(void);			// ブロックの解析
static void VarDecl(void);	// 変数宣言
static void ConstDecl(void);		// 定数宣言
static void FuncDecl(void);			// 関数宣言
static void ClassDecl(void);		// クラス宣言
static void ClassFuncDecl(void);	// クラス内関数宣言
static void ObjectDecl(void);		// オブジェクト宣言

static void ConstRactRun(char *ObjectName); // コンストラクタ実行
static void DestRactRun(char *ObjectName); // デストラクタ実行
static int VarRead(char *Name,int *Pointer,int *Array); // 変数宣言を読み取る

int GrammerAll(void)
{
	TNO = 0;
	CodeInit();
	TableInit();

	TableBlockBegin();

	Block();
	CodeAdd(VC_END,0,0);

	return 0;
}

// ブロックの解析
void Block(void)
{
	CodeAdd(VC_JUMP,0,0);
	int JumpAddress = GetCodePos();

	// 宣言部解析
	while (1)
	{
		switch (Token[TNO].Kind)
		{
		case TK_VAR: // 変数宣言
			TNO++;
			VarDecl();
			continue;
		case TK_CONST: // 定数宣言
			TNO++;
			ConstDecl();
			continue;
		case TK_FUNC: // 関数宣言
			FuncBlock++;
			TNO++;
			FuncDecl();
			continue;
		case TK_CLASS: // クラス宣言
			TNO++;
			ClassDecl();
			continue;
		case TK_OBJECT: // オブジェクト宣言
			TNO++;
			ObjectDecl();
			continue;
		case TK_SEMICOLON:
		case TK_PERIOD:
			TNO++;
			continue;
		default:
			break; // switch脱出
		}
		break; // while脱出
	}		

	CodeRevision(JumpAddress,GetCodePos() + 1 - FuncBlockCode[FuncBlock],0);
	Statement(); // 文解析
	FuncBlockCode[FuncBlock] = 0;

	// デストラクタ処理
	int i;
	for (i = 0;i < FuncBlockDest[FuncBlock];i++) {
		DestRactRun(FuncBlockDestName[FuncBlock][i]);
	}
	FuncBlockDest[FuncBlock] = 0;

	// 関数復帰
	if (FuncBlock > 0) {
		FuncBlock--;
		if (Code[GetCodePos()].VCode != VC_RETURN) CodeAdd(VC_RETURN,0,0);
	}

	CodeAdd(VC_NON,0,0);
}

// 変数宣言(var i,j;)
void VarDecl(void)
{
	// 変数読み取り
	char Name[1024];
	int End,Pointer,Array;

	do {
		End = VarRead(Name,&Pointer,&Array);
		TableAdd(NK_VAR,Name,Pointer);
		TableAddArray(Array);
	} while (End);

	CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);
}

// コンストラクタ
void ConstRactRun(char *ObjectName)
{
	char FuncName[1024];

	NameInfo ObjName = TableGetNameInfo(ObjectName);
	ClassInfo ClassData = TableGetClassInfo(ObjName.Class);
	sprintf(FuncName,"%s_%s",ClassData.Name,ClassData.Name);
	CodeAdd(VC_SAVE,OBJADDRESS,ObjName.Address);

	NameInfo Name = TableGetNameInfo(FuncName);

	CodeAdd(VC_CALL,Name.Address,0);

	FuncBlockCode[FuncBlock] += 2;
}

// デストラクタ
void DestRactRun(char *ObjectName)
{
	char FuncName[1024];

	NameInfo ObjName = TableGetNameInfo(ObjectName);
	ClassInfo ClassData = TableGetClassInfo(ObjName.Class);
	sprintf(FuncName,"%s_~%s",ClassData.Name,ClassData.Name);
	CodeAdd(VC_SAVE,OBJADDRESS,ObjName.Address);

	NameInfo Name = TableGetNameInfo(FuncName);

	CodeAdd(VC_CALL,Name.Address,0);
}

// オブジェクト宣言
void ObjectDecl(void)
{
	int ClassNo;
	int ConstRact = 0;
	int DestRact = 0;
	NameInfo Name;

	if (Token[TNO].Kind == TK_NAME) {
		ClassNo = TableAddVarClass(Token[TNO].Name); // 今後宣言する変数をこのクラスの型に指定
		if (ClassNo == 0) ErrorSet(ERROR_NAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		TNO++;
	} else {
		ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
	}

	// コンストラクタ判定
	char FuncName[1024];
	ClassInfo ClassData = TableGetClassInfo(ClassNo);
	sprintf(FuncName,"%s_%s",ClassData.Name,ClassData.Name);
	if (TableGetNameNo(FuncName) != -1) {
		Name = TableGetNameInfo(ClassData.Name);
		ConstRact = 1;
	}

	// デストラクタ判定
	sprintf(FuncName,"%s_~%s",ClassData.Name,ClassData.Name);
	if (TableGetNameNo(FuncName) != -1) {
		Name = TableGetNameInfo(ClassData.Name);
		DestRact = 1;
	}

	// 変数読み取り
	char sName[1024];
	int End,Pointer,Array;

	do {

		End = VarRead(sName,&Pointer,&Array);
		TableAdd(NK_VAR,sName,Pointer);
		TableAddArray(Array);

		if (ConstRact == 1) ConstRactRun(sName);
		if (DestRact == 1) {
			strcpy(FuncBlockDestName[FuncBlock][FuncBlockDest[FuncBlock]],sName);
			FuncBlockDest[FuncBlock]++;
		}

	} while (End);

	CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);

	TableAddVarClass(NULL); // クラス型指定取り消し
}

// 定数宣言(const m = 10,n = 5;)
void ConstDecl(void)
{
	int NameNo;

	while (1) {

		if (Token[TNO].Kind == TK_NAME) {

			NameNo = TNO;
			TNO++;

			CheckToken(TK_EQUAL,ERROR_EQUAL);

			if (Token[TNO].Kind == TK_NUMBER) {
				// 定数登録
				TableAdd(NK_CONST,Token[NameNo].Name,Token[TNO].Value);
				TNO++;
			} else {
				ErrorSet(ERROR_NUMBER,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
			}

		} else {
			ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		}

		// カンマチェック
		if (Token[TNO].Kind != TK_COMMA) {
			if (Token[TNO].Kind == TK_NAME){
				ErrorSet(ERROR_COMMA,Token[TNO].Line,Token[TNO].Count,NULL,NULL,0,0);
				continue;
			} else {
				break;
			}
		}

		TNO++;

	}

	CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);

}

// 関数宣言(function func(x,y)
void FuncDecl(void)
{
	if (Token[TNO].Kind != TK_NAME) {
		ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
		return;
	}

	// クラス区別
	char ClassName[1024];
	char FuncName[1024];
	strcpy(ClassName,Token[TNO].Name);

	char *Class = strchr(ClassName,'_');
	if (Class != NULL) {
		*Class = '\0';
		strcpy(FuncName,&Token[TNO].Name[strlen(ClassName) + 1]);
	}

	// 関数登録
	int FuncNameNo = TableAdd(NK_FUNC,Token[TNO].Name,GetCodePos() + 1);

	TNO++;
	CheckToken(TK_LPAREN,ERROR_LPAREN);

	TableBlockBegin();

	// パラメータ読み取り
	if (Token[TNO].Kind != TK_RPAREN) {

		char Name[1024];
		int End,Pointer,Array;

		do {
			End = VarRead(Name,&Pointer,&Array);
			TableFuncParAddress(FuncNameNo,TableAdd(NK_VAR,Name,Pointer));
		} while (End);
	}

	CheckToken(TK_RPAREN,ERROR_RPAREN);

	if (Class != NULL) ClassFuncNo = TableGetClassNo(ClassName);
	Block();
	ClassFuncNo = 0;

	CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);
}

// クラス宣言
void ClassDecl()
{
	int FuncMode = 0;
	int ClassNo;

	if (Token[TNO].Kind == TK_NAME) {
		
		ClassNo = TableAdd(NK_CLASS,Token[TNO].Name,0);
		TNO++;
		
		while(1) {
			
			if (Token[TNO].Kind == TK_ENDTYPE) {
				TNO++;
				CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);
				return;
			}

			if (Token[TNO].Kind == TK_FUNC) {
				FuncMode = 1;
				TNO++;
				CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);
				continue;
			}

			// メンバー読み取り
			if (Token[TNO].Kind == TK_NAME) {
				if (FuncMode == 1) {
					// 関数宣言
					ClassFuncDecl();
				} else {

					char Name[1024];
					int End,Pointer,Array;

					do {
						End = VarRead(Name,&Pointer,&Array);
						TableAddClass(NK_VAR,Name,Pointer);
						TableAddArray(Array);
					} while (End);

				}
			}

			CheckToken(TK_SEMICOLON,ERROR_SEMICOLON);
		}
		
	} else {
		ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
	}
}

// クラス内関数宣言
void ClassFuncDecl()
{
	if (Token[TNO].Kind == TK_NAME) {

		//int FuncNameNo = TableAdd(NK_FUNC,Token[TNO].Name,GetCodePos() + 1);
		TNO++;
		CheckToken(TK_LPAREN,ERROR_LPAREN);

		// パラメータ読み取り
		if (Token[TNO].Kind != TK_RPAREN) {

			char Name[1024];
			int End,Pointer,Array;

			do {
				End = VarRead(Name,&Pointer,&Array);
			} while (End);
		}
			
		CheckToken(TK_RPAREN,ERROR_RPAREN);

	} else {
		ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
	}
}

// 変数宣言を読み取る。戻り値が１の間は読み続けること。
int VarRead(char *Name,int *Pointer,int *Array)
{
	if (Token[TNO].Kind == TK_NAME) {
		TNO++;
		// 変数登録
		if (Token[TNO].Kind == TK_POINTER) { // ポインタ
			*Pointer = 1;
			*Array = 0;
			strcpy(Name,Token[TNO - 1].Name);
			TNO++;
		} else if (Token[TNO].Kind == TK_LARRAY) { // 配列

			*Pointer = 1;
			NUMLOG(Token[TNO - 1].Name);
			strcpy(Name,Token[TNO - 1].Name);

			TNO++;
			if (Token[TNO].Kind == TK_NUMBER) {
				*Array = Token[TNO].Value;
				TNO++;
			} else {
				*Array = 0;
			}

			CheckToken(TK_RARRAY,ERROR_RARRAY);
		} else { // 通常変数
			*Pointer = 0;
			*Array = 0;
			strcpy(Name,Token[TNO - 1].Name);
		}
	} else {
		ErrorSet(ERROR_NONAME,Token[TNO].Line,Token[TNO].Count,Token[TNO].Name,NULL,0,0);
	}

	// カンマチェック
	if (Token[TNO].Kind != TK_COMMA) {
		if (Token[TNO].Kind == TK_NAME){
			ErrorSet(ERROR_COMMA,Token[TNO].Line,Token[TNO].Count,NULL,NULL,0,0);
			return 1;
		} else {
			return 0;
		}
	}

	TNO++;

	return 1;
}