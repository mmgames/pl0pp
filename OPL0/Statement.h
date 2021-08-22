#ifndef __INCLUDE_STATMENT__
#define __INCLUDE_STATMENT__

#include "Error.h"

#define OBJADDRESS 1000 // メンバー関数呼び出し時にオブジェクトのアドレスを記憶する位置

extern int TNO; // トークンの位置
extern int ClassFuncNo;	// 処理しているメンバー関数

// 文の構文解析を行う
int Statement(void);

// あるべき単語が存在するかチェック
void CheckToken(TokenKind Kind,ERRORTYPE No);

#endif