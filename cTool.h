#ifndef _CTOOL_H_
#define _CTOOL_H_
#include<windows.h>
#include<stdio.h>
#define PROGRESS_BAR_LEN 12
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ENTER 13
#define KEY_ESC   27
/// <summary>
/// 光标定位函数
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
void gotoxy(int x, int y);

/// <summary>
/// 窗口绘制函数
/// </summary>
/// <param name="startx">起始点x坐标</param>
/// <param name="starty">起始点y坐标</param>
/// <param name="width">窗体宽度</param>
/// <param name="heigth">窗体高度</param>
void paintWin(int startx, int starty, int width, int height);

/// <summary>
/// 字符串输入控制函数
/// </summary>
/// <param name="maxlen">最多输入字符长度</param>
/// <param name="inputType">1-字母 2-数字</param>
/// <param name="ips">1-明文 2-密文</param>
/// <param name="str">保存到外界的字符数组/自定义内存空间</param>
void glbString(int maxlen, int inputType, int ips, char str[]);
/// <summary>
/// 获取键盘按键，用来返回对应的按键的值
/// </summary>
/// <returns></returns>
int getKey();
#endif