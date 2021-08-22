#ifndef _INCLUDE_TABLE_
#define _INCLUDE_TABLE_

#include "Token.h"

#define MAXLEVEL 20
#define MAXTABLE 256
#define MAXCLASSVAR 30

// 名前の種類
enum NameKind {
	NK_NON,			// 不明な種類
	NK_VAR,			// 変数
	NK_FUNC,		// 関数
	NK_CONST,		// 定数
	NK_CLASS,		// クラス型
	NK_OBJECT,		// オブジェクト
};

// 名前情報
typedef struct {
	NameKind Kind;		// 種類
	char Name[MAXNAME]; // 名前
	int Pointer;		// ポインタか否か
	int Array;			// 配列の要素数。０なら単一変数。
	int Value;			// 値
	int Address;		// アドレス
	int Pars;			// パラメータ数
	int ParAddress[20];	// 各パラメータのアドレス
	int Class;			// クラス番号
} NameInfo;

// クラス情報
typedef struct {
	char Name[MAXNAME];		// クラス名
	int Var;				// 要素数
	int Func;				// メンバー関数数
	NameInfo NameTable[MAXCLASSVAR];	// 各要素の情報
	int NameCount;			// 名前の個数
	int Addres;				// 構造体内での相対アドレス
} ClassInfo;

// テーブルの初期化
void TableInit();

// 新しいブロックの開始
void TableBlockBegin();

// 現在のブロックの終了
void TableBlockEnd();

// 新しい名前の追加
int TableAdd(NameKind Kind,const char *Name,int Value);

// クラスへの新しい名前の追加
int TableAddClass(NameKind Kind,const char *Name,int Value);

// 直前に登録した変数を配列に設定
void TableAddArray(int Count);

// これから宣言するクラスの型
int TableAddVarClass(const char *Name);

// 関数のパラメータを設定
void TableFuncParAddress(int FuncNameCount,int VarNameCount);

// 関数のパラメータのアドレスを取得
int TableGetFuncParAddress(int FuncNameCount,int ParCount);

// 既存の名前の番号を取得
int TableGetNameNo(const char *Name);

// 既存の名前の情報を取得
NameInfo TableGetNameInfo(const char *Name);

// 既存のクラスの番号を取得
int TableGetClassNo(const char *Name);

// 既存のクラスの情報を取得
ClassInfo TableGetClassInfo(int ClassNo);

// クラスから既存の名前の番号を取得
int TableGetClassNameNo(int ClassNo,const char *Name);

// クラスから既存の名前の情報を取得
NameInfo TableGetClassNameInfo(int ClassNo,const char *Name);

#endif