#ifndef _INCLUDE_FILESOURCE_
#define _INCLUDE_FILESOURCE_

// 指定されたプログラムを記憶する
void FileSourceText(char *Buf);

// 次の文字を取り出す
char FileSourceNextChar(void);

// 現在の行を得る
int FileSourceGetLine(void);

// 現在の文字を得る
int FileSourceGetCount(void);

#endif