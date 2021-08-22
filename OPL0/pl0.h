// コンパイラとのインタフェース
#ifndef __INCLUDE_PL0__
#define __INCLUDE_PL0__

// プログラムをコンパイルし、実行結果を返す
int GetResult(char *Program,char *Result);

// エラーメッセージの取得
int GetErrorText(char *Error);

#endif
