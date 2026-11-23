#include"cWindows.h"
#include"cTool.h"
#include<conio.h>

// 显示窗口
void showWin(WIN_T win) {
	paintWin(win.startx, win.starty, win.width, win.height);
	//遍历所有的控件进行处理
	for (int i = 0; i < win.ctrlCount; i++) {
		if (win.ctrlArr[i].ctrlType == LABEL) {
			gotoxy(win.ctrlArr[i].startx, win.ctrlArr[i].starty);
			printf("%s", win.ctrlArr[i].content);
		}
		else if (win.ctrlArr[i].ctrlType == EDIT) {
			paintWin(win.ctrlArr[i].startx, win.ctrlArr[i].starty, win.ctrlArr[i].width, win.ctrlArr[i].height);
			gotoxy(win.ctrlArr[i].startx, win.ctrlArr[i].starty);
			printf("%s", win.ctrlArr[i].content);
		}
	}
}

// 添加控件
int controlAdd(CONTROL_T ctrlArr[],CONTROL_T ctrl,int count) {
	ctrlArr[count++] = ctrl;
	return count;
}

// 运行窗口
void winRun(WIN_T* win)
{
	int key = 0;
	int i = 0;
	// 寻找到第一个可以可交互的控件（编辑框/按钮）
	for (i = 0; i < win->ctrlCount; i++)
	{
		if (win->ctrlArr[i].ctrlType == EDIT || win->ctrlArr[i].ctrlType == BUTTON)
		{
			gotoxy(win->ctrlArr[i].startx + 2, win->ctrlArr[i].starty + 1);
			break;
		}
	}

	while (1)
	{
		key = getKey();
		switch (key)
		{
		case KEY_DOWN:
		{
			i++;
			if (i == win->ctrlCount)
			{
				i = 0;
			}
			for (; i < win->ctrlCount; i++)
			{
				if (win->ctrlArr[i].ctrlType == BUTTON)
				{
					gotoxy(win->ctrlArr[i].startx + 2, win->ctrlArr[i].starty + 1);
					break;
				}
				else if (win->ctrlArr[i].ctrlType == EDIT)
				{
					gotoxy(win->ctrlArr[i].startx + 2, win->ctrlArr[i].starty + 1);
					break;
				}
			}

			break;
		}
		case KEY_UP:
		{
			i--;
			for (; i >= 0; i--)
			{
				if (win->ctrlArr[i].ctrlType == BUTTON)
				{
					gotoxy(win->ctrlArr[i].startx + 2, win->ctrlArr[i].starty + 1);
					break;
				}
				else if (win->ctrlArr[i].ctrlType == EDIT)
				{
					gotoxy(win->ctrlArr[i].startx + 2, win->ctrlArr[i].starty + 1);
					break;
				}
			}
			if (i == 0)
			{
				i = win->ctrlCount;
			}

			break;
		}
		case KEY_ENTER:
		{
			if (win->ctrlArr[i].ctrlType == BUTTON)
			{
				win->ctrlIndex = i;
				return;
			}

			break;
		}
		default:
		{
			if (win->ctrlArr[i].ctrlType == EDIT)
			{
				//glbString(win->ctrlArr[i].maxLen,win->ctrlArr[i].inputType,win->ctrlArr[i].ips,win->ctrlArr[i].content);
				keyEditListen(win->ctrlArr[i], win->ctrlArr[i].content, key);
			}

			break;
		}
		}
	}
}


// 处理编辑框输入
void keyEditListen(CONTROL_T ctrl, char str[], char ch)
{
	size_t len = strlen(str);
	if (len > 0 && ch == '\b')
	{
		printf("\b \b");
		str[--len] = '\0';
	}

	if (ctrl.maxLen <= 0 || len >= (size_t)ctrl.maxLen)
	{
		return;
	}

	if (ctrl.inputType == 1)
	{
		if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		{
			if (ctrl.ips == 1)
			{
				_putch(ch);
			}
			if (ctrl.ips == 2)
			{
				_putch('*');
			}
			str[len++] = ch;
			str[len] = '\0';
		}
	}
	if (ctrl.inputType == 2)
	{
		if (ch >= '0' && ch <= '9')
		{
			if (ctrl.ips == 1)
			{
				_putch(ch);
			}
			if (ctrl.ips == 2)
			{
				_putch('*');
			}
			str[len++] = ch;
			str[len] = '\0';
		}
	}


}
