#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "FileSource.h"
#include "Token.h"
#include "Grammar.h"
#include "Code.h"
#include "Error.h"
#include "LogWindow.h"

int GetResult(char *Program,char *Result)
{	
	setlocale(LC_ALL, "jpn"); // 日本語で処理

	// ソースファイルの読み込み
	FileSourceText(Program);

	// コンパイルの実行
	// 字句解析（単語の格納）
	TokenAll();

	// 構文解析＆目的語生成（機械語の生成）
	try {
		ErrorInit();
		GrammerAll();
		CodeShow();
	} catch(...) {
		TXTLOG("原因不明エラーに遭遇したため、コンパイルを強制中止します。");
		return -1;
	}

	// インタプリタの実行
	try {
		Interpreter(Result,0);
	} catch(...) {
		TXTLOG("インタプリタ実行中に例外発生。プログラムに致命的なバグがあると思われます。");
		return -1;
	}

	return 0;
}

// エラーメッセージの取得
int GetErrorText(char *Error)
{
	int i = 0;
	char ErrorStr[128];

	try {
		while (1) {
			if (ErrorGetStr(i,ErrorStr) == 0) break;
			strcat(Error,ErrorStr);
			i++;
		}
	} catch(...) {
		return i;
	}

	return i;
}