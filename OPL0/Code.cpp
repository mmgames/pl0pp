#include <stdio.h>
#include <string.h>
#include "Code.h"
#include "Table.h"
#include "LogWindow.h"

static int CodePos = -1;
CodeInfo Code[MIDDLECODE_MAX];

// 初期化
void CodeInit(void)
{
	CodePos = -1;
	memset(Code,0,sizeof(Code));
}

// 直前に追加した命令のアドレス
int GetCodePos(void) { return CodePos; }

// 新しい命令を追加
void CodeAdd(VirtualCode VCode,int DataA,int DataB)
{
	CodePos++;
	Code[CodePos].VCode = VCode;
	Code[CodePos].DataA = DataA;
	Code[CodePos].DataB = DataB;
}

// 既存の命令を修正する(バックパッチ)
void CodeRevision(int No,int DataA,int DataB)
{
	Code[No].DataA = DataA;
	Code[No].DataB = DataB;

	// 無意味なジャンプ命令を削除
	if (Code[No].VCode == VC_JUMP && Code[No].DataA == No + 1) {
		Code[No].VCode = VC_NON;
		Code[No].DataA = 0;
		Code[No].DataB = 0;
	}
}

// 各命令の文字列を取得
inline void CodeStr(VirtualCode VCode,char *Str)
{
	switch (VCode)
	{
		case VC_SAVE: strcpy(Str,"記憶"); return;
		case VC_COPY: strcpy(Str,"コピー"); return;
		case VC_POUT: strcpy(Str,"参照出力"); return;
		case VC_PINT: strcpy(Str,"参照入力"); return;
		case VC_CALC: strcpy(Str,"計算"); return;
		case VC_PRINT: strcpy(Str,"表示"); return;
		case VC_NEWLINE: strcpy(Str,"改行"); return;
		case VC_CALL: strcpy(Str,"関数呼出"); return;
		case VC_RETURN: strcpy(Str,"関数復帰"); return;
		case VC_JUMP: strcpy(Str,"ジャンプ"); return;
		case VC_JUMP0: strcpy(Str,"０ジャンプ"); return;
		case VC_NON: strcpy(Str,"何もしない"); return;
		case VC_END: strcpy(Str,"終了"); return;
	}
}

// 各演算子の文字列を取得
inline void OperStr(int Oper,char *Str)
{	switch (Oper)
	{
		case OR_NEG: strcpy(Str,"反転"); return;
		case OR_ADD: strcpy(Str,"＋"); return;
		case OR_SUB: strcpy(Str,"－"); return;
		case OR_MUL: strcpy(Str,"Ｘ"); return;
		case OR_DIV: strcpy(Str,"÷"); return;

		case OR_ODD: strcpy(Str,"奇数"); return;
		case OR_EQ: strcpy(Str,"＝"); return;
		case OR_LS: strcpy(Str,"＜"); return;
		case OR_GR: strcpy(Str,"＞"); return;
		case OR_NEQ: strcpy(Str,"！＝"); return;
		case OR_LSEQ: strcpy(Str,"＜＝"); return;
		case OR_GREQ: strcpy(Str,"＞＝"); return;
	}
}

// 命令リストを表示
void CodeShow()
{
	int i;
	char SCode[256],SA[256],SB[256];

	for (i = 0;i <= CodePos;i++)
	{
		switch (Code[i].VCode) {

			case VC_POUT:
			case VC_PINT:
			case VC_SAVE:
				if (Code[i].DataA == STOCK) strcpy(SA,"スタック"); else sprintf(SA,"(%d)",Code[i].DataA);
				sprintf(SB,"%d",Code[i].DataB);
				break;
			case VC_COPY:
				if (Code[i].DataA == STOCK) strcpy(SA,"スタック"); else sprintf(SA,"(%d)",Code[i].DataA);
				if (Code[i].DataB == STOCK) strcpy(SB,"スタック"); else sprintf(SB,"(%d)",Code[i].DataB);
				break;
			case VC_CALC:
				OperStr(Code[i].DataA,SA);
				SB[0] = '\0';
				break;
			case VC_JUMP:
			case VC_JUMP0:
			case VC_CALL:
				sprintf(SA,"[%d]",Code[i].DataA);
				SB[0] = '\0';
				break;
			case VC_RETURN:
			case VC_PRINT:
			case VC_NEWLINE:
			case VC_NON:
			case VC_END:
				SA[0] = '\0';
				SB[0] = '\0';
				break;

		}

		CodeStr(Code[i].VCode,SCode);
		TXTLOG("[%3d]%11s %10s %10s",i,SCode,SA,SB);
	}
}