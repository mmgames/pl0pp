#ifndef _INCLUDE_MIDDLECODE_
#define _INCLUDE_MIDDLECODE_

#define MIDDLECODE_MAX 1024
#define STOCK -1	//スタックの頂上を意味する

enum VirtualCode {	// 仮想命令
	VC_SAVE,	// 定数値を記憶する：記憶先、数値
	VC_COPY,	// メモリからコピーする：コピー元、コピー先
	VC_POUT,		// ポインタから値をスタックにコピーする：ポインタ
	VC_PINT,		// スタックからポインタに値をコピーする：ポインタ
	VC_CALC,	// スタック値を計算する：演算子
	VC_PRINT,	// スタック値を表示する
	VC_NEWLINE,	// 改行する
	VC_CALL,	// 関数呼び出し：ジャンプ先
	VC_RETURN,	// 関数呼び出しからの復帰
	VC_JUMP,	// 無条件ジャンプ：ジャンプ先
	VC_JUMP0,	// スタック値が０の時ジャンプ：ジャンプ先
	VC_NON,		// 何もしない
	VC_END,		// プログラムの強制終了
};

enum Operation {	// 演算子一覧

	OR_NEG,		// 符号反転
	OR_ADD,		// 加算
	OR_SUB,		// 減算
	OR_MUL,		// 乗算
	OR_DIV,		// 除算

	OR_ODD,		// 奇数の時１
	OR_EQ,		// 等しい時１
	OR_LS,		// 左が小さい時１
	OR_GR,		// 右が小さい時１
	OR_NEQ,		// 異なるとき１
	OR_LSEQ,	// 左が以下の時１
	OR_GREQ,	// 右が以下の時１
};

typedef struct {	// 命令情報
	VirtualCode VCode;	// 命令
	int DataA;			// 第１パラメータ
	int DataB;			// 第２パラメータ
} CodeInfo;

// 命令リスト
extern CodeInfo Code[MIDDLECODE_MAX];

// 初期化 
void CodeInit(void);

// 新しい命令を追加
void CodeAdd(VirtualCode Code,int DataA,int DataB);

// 既存の命令を修正する(バックパッチ)
void CodeRevision(int No,int DataA,int DataB);

// 直前に追加した命令のアドレス
int GetCodePos(void);

// 命令リストを表示
void CodeShow();

// 命令を実行
void Interpreter(char *Result,int StackView = 0);

#endif