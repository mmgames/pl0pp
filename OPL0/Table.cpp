#include <stdio.h>
#include <string.h>
#include "Table.h"
#include "Token.h"
#include "Error.h"
#include "LogWindow.h"

// 名前情報リスト
static NameInfo NameTable[MAXTABLE];

// クラス情報リスト
static ClassInfo ClassTable[MAXTABLE];

static int NameCount = -1;		// 上位ブロック内も含む名前の個数
static int Addres = -1;			// グローバル領域内で変数を確保したアドレス
static int BlockName[MAXLEVEL];	// 各ブロックの名前の保存位置
static int Block = -1;			// 現在のブロック
static int ClassCount = 0;		// 現在のクラスの番号
static int VarClassNo = 0;	// 宣言中のクラス

// テーブルの初期化
void TableInit()
{
	NameCount = -1;
	Addres = -1;
	Block = -1;

	memset(NameTable,0,sizeof(NameTable));
	memset(BlockName,0,sizeof(BlockName));
}

// 新しいブロックの開始
void TableBlockBegin()
{
	if (Block != -1) BlockName[Block] = NameCount;	// 現在のブロックの名前の位置を保存
	Block++;
	return;
}

// 現在のブロックの終了
void TableBlockEnd()
{
	Block--;
	if (Block != -1) NameCount = BlockName[Block];	// 以前のブロックの名前の位置を復元
}

// 新しい名前の追加
void TableAddName(NameKind Kind,const char *Name,int Value,int NameCount,int *Addres,NameInfo *NameTable)
{
	NameTable->Kind = Kind;
	strcpy(NameTable->Name, Name);

	switch (Kind) {
		case NK_VAR:
			(*Addres)++;
			NameTable->Address = *Addres;
			NameTable->Pointer = Value;
			NameTable->Class = VarClassNo;
			TXTLOG("[ 変数 ]:%3d 番,%3d アドレス,%3d ブロック,%d 番クラス,%s,%s 名前",NameCount,*Addres,Block,VarClassNo,(Value ? "ポインタ" : "通常変数"),Name);
			if (VarClassNo != 0) (*Addres) += ClassTable[VarClassNo].Addres;
			break;
		case NK_CONST:
			NameTable->Value = Value;
			TXTLOG("[ 定数 ]:%3d 番,%3d ブロック,%3d 値,%s 名前",NameCount,Block,Value,Name);
			break;
		case NK_FUNC:
			NameTable->Address = Value;
			TXTLOG("[ 関数 ]:%3d 番,%3d アドレス,%s 名前",NameCount,Value,Name);
			break;
		case NK_CLASS:
			ClassCount++;
			strcpy(ClassTable[ClassCount].Name,Name);
			ClassTable[ClassCount].NameCount = -1;
			ClassTable[ClassCount].Addres = -1;
			ClassTable[ClassCount].Func = 0;
			ClassTable[ClassCount].Var = 0;
			TXTLOG("[クラス]:%3d 番,%d 番クラス,%s 名前",NameCount,ClassCount,Name);
			break;
	}
}

// 新しい名前の追加
int TableAdd(NameKind Kind,const char *Name,int Value)
{
	NameCount++;
	TableAddName(Kind,Name,Value,NameCount,&Addres,&NameTable[NameCount]);
	return NameCount;
}

// クラスへの新しい名前の追加
int TableAddClass(NameKind Kind,const char *Name,int Value)
{
	ClassTable[ClassCount].NameCount++;
	TableAddName(Kind,Name,Value,
		ClassTable[ClassCount].NameCount,
		&ClassTable[ClassCount].Addres,
		&ClassTable[ClassCount].NameTable[ClassTable[ClassCount].NameCount]);
	return ClassTable[ClassCount].NameCount;
}

// これから宣言するクラスの型
int TableAddVarClass(const char *Name)
{
	if (Name != NULL) {
		VarClassNo = TableGetClassNo(Name);
	} else {
		VarClassNo = 0;
	}

	return VarClassNo;
}

// 直前に登録した変数を配列に設定
void TableAddArray(int Count)
{
	if (Count == 0) return;
	Addres += Count; // 要素数よりも１つ多く確保している
	NameTable[NameCount].Array = Count;
	TXTLOG("上記変数を配列に指定 %3d 要素数",Count);
}

// 関数のパラメータを設定
void TableFuncParAddress(int FuncNameCount,int VarNameCount)
{
	NameTable[FuncNameCount].ParAddress[NameTable[FuncNameCount].Pars] = NameTable[VarNameCount].Address;
	TXTLOG("[ 引数 ]:%s 関数,%3d 番,%s 変数,%3d アドレス",NameTable[FuncNameCount].Name,NameTable[FuncNameCount].Pars,NameTable[VarNameCount].Name,NameTable[VarNameCount].Address);
	NameTable[FuncNameCount].Pars++;
}

// 関数のパラメータのアドレスを取得
int TableGetFuncParAddress(int FuncNameCount,int ParCount)
{
	return NameTable[FuncNameCount].ParAddress[ParCount];
}

// 既存の名前の番号を取得
int TableGetNameNo(const char *Name)
{
	int i;
	
	// 後ろから探すことでブロックの優先順位に従う
	for (i = NameCount;i >= 0;i--) {
		if (strcmp(Name, NameTable[i].Name) == 0) return i;
	}
	
	return -1;
}

// 既存の名前の情報を取得
NameInfo TableGetNameInfo(const char *Name)
{
	int No = TableGetNameNo(Name);

	if (No == -1) {
		NameInfo Name = {NK_NON};
		return Name;
	}

	return NameTable[No];
}

// 既存のクラスの番号を取得
int TableGetClassNo(const char *Name)
{
	int i;
	
	// 後ろから探すことでブロックの優先順位に従う
	for (i = 1;i <= ClassCount;i++) {
		if (strcmp(Name, ClassTable[i].Name) == 0) return i;
	}
	
	return 0;
}

// 既存のクラスの情報を取得
ClassInfo TableGetClassInfo(int ClassNo)
{
	return ClassTable[ClassNo];
}

// クラスから既存の名前の番号を取得
int TableGetClassNameNo(int ClassNo,const char *Name)
{
	int i;
	
	// 後ろから探すことでブロックの優先順位に従う
	for (i = NameCount;i >= 0;i--) {
		if (strcmp(Name, ClassTable[ClassNo].NameTable[i].Name) == 0) return i;
	}
	
	return -1;
}

// クラスから既存の名前の情報を取得
NameInfo TableGetClassNameInfo(int ClassNo,const char *Name)
{
	int No = TableGetClassNameNo(ClassNo,Name);

	if (No == -1) {
		NameInfo Name = {NK_NON};
		return Name;
	}

	return ClassTable[ClassNo].NameTable[No];
}