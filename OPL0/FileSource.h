#ifndef _INCLUDE_FILESOURCE_
#define _INCLUDE_FILESOURCE_

// �w�肳�ꂽ�v���O�������L������
void FileSourceText(char *Buf);

// ���̕��������o��
char FileSourceNextChar(void);

// ���݂̍s�𓾂�
int FileSourceGetLine(void);

// ���݂̕����𓾂�
int FileSourceGetCount(void);

#endif