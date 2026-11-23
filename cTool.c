#include"cTool.h"
#include<stdio.h>
#include<conio.h>

// 移动光标
void gotoxy(int x, int y)//形参
{
	HANDLE hOut;
	COORD pos = { x,y };
	// 光标的起始位（第1列，第3行） 0是第1列 2是第3行
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOut, pos);
	//printf("定位光标位置搜索(%d,%d)\n",pos.X,pos.Y);
}

// 绘制窗口
void paintWin(int startx, int starty, int width, int height) {
	gotoxy(startx, starty);

	//第一行
	printf("┏");
	for (int i = 0; i < width - 1; i++) {
		printf("━");
	}
	printf("┓");

	//中间行
	gotoxy(startx, starty + 1);
	for (int i = 1; i < height - 1; i++) {
		gotoxy(startx, starty + i);
		printf("┃");
		gotoxy(startx + width, starty + i);
		printf("┃");
	}

	//最后一行
	gotoxy(startx, starty+height - 1);
	printf("┗");
	for (int i = 0; i < width - 1; i++) {
		printf("━");
	}
	printf("┛");
}


// 读取字符串
void glbString(int maxlen, int inputType, int ips, char str[]) {
	char ch = '\0';
	int len = 0;
	while (1) {
		ch = _getch();
		if ((len > 0 && ch == '\r') || (len == maxlen)) {
			break;
		}
		if (len == 0 && ch == '\r') {
			continue;
		}
		if (ch == '\b' && len > 0) {
			printf("\b \b");
			str[--len] = '\0';
		}
		if (inputType == 1) {
			if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
				_putch(ch);
				str[len++] = ch;
			}
		}
		if (inputType == 2) {
			if (ch >= '0' && ch <= '9') {
				_putch(ch);
				str[len++]=ch;
			}
		}
		str[len++] = ch;
	}
}

// 读取按键
int getKey()
{
	int ch = 0;
	ch = _getch();

	if (ch == 0 || ch == 224)
	{
		ch = _getch();
		if (ch == 72)
		{
			return KEY_UP;
		}
		else if (ch == 80)
		{
			return KEY_DOWN;
		}
		else if (ch == 75)
		{
			return KEY_LEFT;
		}
		else if (ch == 77)
		{
			return KEY_RIGHT;
		}
	}
	else if (ch == 13)
	{
		return KEY_ENTER;
	}
	else if (ch == 27)
	{
		return KEY_ESC;
	}

	return 0;
}
