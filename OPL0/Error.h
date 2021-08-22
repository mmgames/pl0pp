#ifndef __INCLUDE_ERROR__
#define __INCLUDE_ERROR__

#define ERROR_MAX 100

enum ERRORTYPE {
	ERROR_UNKNOW,		// 原因不明エラー
	ERROR_NONAME,		// 名前ではない
	ERROR_NUMBER,		// 数値ではない
	ERROR_EQUAL,		// イコールがない
	ERROR_SEMICOLON,	// セミコロンがない
	ERROR_DO,			// DOがない
	ERROR_THEN,			// THENがない
	ERROR_LPAREN,		// 左括弧がない
	ERROR_RPAREN,		// 右括弧がない
	ERROR_LARRAY,		// [ がない
	ERROR_RARRAY,		// ] がない
	ERROR_COMMA,		// カンマがない
	ERROR_PERIOD,		// . がない
	ERROR_TOKEN,		// 無意味な単語
	ERROR_CONDITION,	// 条件演算子ではない
	ERROR_NAME,			// 未定義の名前
	ERROR_CONST,		// 定数への代入
	ERROR_CALCADDRESS,	// 計算できないアドレス
	ERROR_POINTER,		// ポインタ変数ではない
	ERROR_ARRAY,		// 配列ではない
	ERROR_ARRAYINPUT,	// 配列名には代入できない
	ERROR_TYPE,			// 異なる型での代入
};

// エラーを初期化
void ErrorInit(void);

// エラー内容をファイルに書き出す
int ErrorWriteFile(const char *File);

// エラー文字列を取得
int ErrorGetStr(int No,char *Buf);

// 新しいエラーを記憶
int ErrorSet(ERRORTYPE No,int Line,int Count,const char *s1,const char *s2,int i1,int i2);

#endif