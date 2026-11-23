#ifndef _CWINDOW_H_
#define _CWINDOW_H_
#define LABEL  1
#define EDIT   2
#define BUTTON 3
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct control_t {
	int startx;
	int starty;
	int width;
	int height;
	char content[50];//文本内容最多50

	int maxLen;
	int ips;
	int inputType;
	int ctrlType;
} CONTROL_T;

typedef struct win_t {
	int startx;
	int starty;
	int width;
	int height;
	CONTROL_T ctrlArr[20];
	int ctrlCount;
	int ctrlIndex;
}WIN_T;
/// <summary>
/// 显示窗体
/// </summary>
/// <param name="win"></param>
void showWin(WIN_T win);

/// <summary>
/// 添加控件向控件数组中添加一个新控件，返回新控件数量
/// </summary>
/// <param name="ctrlArr"></param>
/// <param name="ctrl"></param>
/// <param name="count"></param>
int controlAdd(CONTROL_T ctrlArr[], CONTROL_T ctrl, int count);

/// <summary>
/// 窗体驱动，用于处理用户输入喝控件交互
/// </summary>
/// <param name="win"></param>
void winRun(WIN_T* win);
/// <summary>
/// 
/// </summary>
/// <param name="ctrl">控件</param>
/// <param name="str">保存的数</param>
/// <param name="ch">按键</param>
void keyEditListen(CONTROL_T ctrl, char str[], char ch);
#endif