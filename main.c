#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <conio.h>
#include <windows.h>
#include <locale.h>
#include "cTool.h"
#include "link.h"
#include "test.h"
#include "database.h"

static char current_user[20] = "";  // 当前登录用户名

static int read_console_line_w(wchar_t* buffer, int capacity)
{
    DWORD read = 0;
    if (!ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), buffer,
        (DWORD)capacity - 1, &read, NULL))
    {
        return 0;
    }

    buffer[read] = L'\0';
    while (read > 0 &&
        (buffer[read - 1] == L'\r' || buffer[read - 1] == L'\n'))
    {
        buffer[--read] = L'\0';
    }
    return 1;
}

static int wide_to_utf8(const wchar_t* source, char* destination, int size)
{
    int written = WideCharToMultiByte(CP_UTF8, 0, source, -1,
        destination, size, NULL, NULL);
    return written > 0 && written <= size;
}

static int read_utf8_line(char* destination, int size)
{
    wchar_t buffer[256];
    if (!read_console_line_w(buffer, 256))
    {
        return 0;
    }
    return wide_to_utf8(buffer, destination, size);
}

static void clear_input_line(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

static void wait_for_key(void)
{
    gotoxy(4, 23);
    printf("按任意键返回主菜单...");
    _getch();
}

static int read_password(char* password, int size)
{
    int length = 0;
    while (length < size - 1)
    {
        int ch = _getch();
        if (ch == '\r')
        {
            break;
        }
        if (ch == '\b')
        {
            if (length > 0)
            {
                length--;
                printf("\b \b");
            }
            continue;
        }
        if (ch >= 32)
        {
            password[length++] = (char)ch;
            _putch('*');
        }
    }
    password[length] = '\0';
    printf("\n");
    return length > 0;
}

// 登录
static int login_screen(void)
{
    USER_T user = { 0 };
    system("cls");
    paintWin(25, 1, 60, 20);
    gotoxy(48, 4);
    printf("学生成绩管理系统登录");

    gotoxy(32, 8);
    printf("用户名：");
    gotoxy(42, 8);
    scanf("%19s", user.name);
    clear_input_line();

    gotoxy(32, 12);
    printf("密  码：");
    gotoxy(42, 12);
    if (!read_password(user.pwd, sizeof(user.pwd)))
    {
        gotoxy(36, 16);
        printf("密码不能为空");
        Sleep(1500);
        return 0;
    }

    if (!login_user(user))
    {
        gotoxy(38, 16);
        printf("用户名或密码错误");
        Sleep(1800);
        return 0;
    }

    strcpy(current_user, user.name);
    gotoxy(42, 16);
    printf("登录成功");
    Sleep(1000);
    return 1;
}

// 注册账号
static void register_screen(void)
{
    USER_T user = { 0 };
    char confirm[20] = "";

    system("cls");
    paintWin(25, 1, 60, 20);
    gotoxy(48, 4);
    printf("注册新账号");

    gotoxy(30, 8);
    printf("用户名：");
    gotoxy(40, 8);
    scanf("%19s", user.name);

    gotoxy(30, 11);
    printf("密  码：");
    gotoxy(40, 11);
    scanf("%19s", user.pwd);

    gotoxy(30, 14);
    printf("确认密码：");
    gotoxy(40, 14);
    scanf("%19s", confirm);
    clear_input_line();

    if (strcmp(user.pwd, confirm) != 0)
    {
        gotoxy(38, 18);
        printf("两次密码不一致");
    }
    else if (user_exists(user.name))
    {
        gotoxy(40, 18);
        printf("用户名已存在");
    }
    else if (!register_user(user))
    {
        gotoxy(38, 18);
        printf("注册失败，请检查 user.txt");
    }
    else
    {
        gotoxy(44, 18);
        printf("注册成功");
    }

    gotoxy(36, 20);
    printf("按任意键返回...");
    _getch();
}

// 录入学生
static STU* input_student(STU* head)
{
    STU student = { 0 };
    wchar_t line[256];
    wchar_t wide_name[NAME_LENGTH] = { 0 };

    system("cls");
    paintWin(5, 1, 90, 22);
    gotoxy(8, 4);
    printf("录入学生成绩");
    gotoxy(8, 7);
    printf("请按顺序输入：学号 姓名 语文 数学 英语");
    gotoxy(8, 10);
    printf("> ");

    if (!read_console_line_w(line, 256))
    {
        return head;
    }

    int matched = swscanf(line, L"%d %19ls %f %f %f",
        &student.num,
        wide_name,
        &student.score[0],
        &student.score[1],
        &student.score[2]);
    int name_length = (int)wcslen(wide_name);

    if (matched != 5 ||
        !wide_to_utf8(wide_name, student.name, sizeof(student.name)) ||
        student.num < 100 || student.num > 999 ||
        name_length < 1 || name_length > 3 ||
        student.score[0] < 0 || student.score[0] > 100 ||
        student.score[1] < 0 || student.score[1] > 100 ||
        student.score[2] < 0 || student.score[2] > 100 ||
        strchr(student.name, '|') != NULL)
    {
        gotoxy(8, 14);
        printf("输入无效：学号必须三位数，姓名最多三个字");
        return head;
    }

    if (find_student_by_num(head, student.num) != NULL)
    {
        gotoxy(8, 14);
        printf("学号 %d 已存在", student.num);
        return head;
    }

    calculate_student_summary(&student);
    head = add_student(head, student);
    if (!save_students(head))
    {
        head = load_students(head);
        gotoxy(8, 15);
        printf("录入失败：数据库保存失败");
        return head;
    }

    gotoxy(8, 15);
    printf("学生记录已同步到MySQL");
    gotoxy(8, 17);
    printf("录入成功，总分 %.2f，平均分 %.2f", student.total, student.average);
    return head;
}

// 按学号查询
static void query_by_num(STU* head)
{
    int num = 0;
    gotoxy(4, 4);
    printf("请输入要查询的学号：");
    if (scanf("%d", &num) != 1)
    {
        clear_input_line();
        printf("\n学号格式错误\n");
        return;
    }
    clear_input_line();

    STU* student = find_student_by_num(head, num);
    if (student == NULL)
    {
        printf("\n未找到学号 %d\n", num);
        return;
    }

    printf("\n学号：%d\n", student->num);
    printf("姓名：%s\n", student->name);
    printf("排名：第 %d 名\n", student_rank(head, student));
    printf("语文：%.2f  数学：%.2f  英语：%.2f\n",
        student->score[0], student->score[1], student->score[2]);
    printf("总分：%.2f  平均分：%.2f\n",
        student->total, student->average);
}

// 按姓名查询
static void query_by_name(STU* head)
{
    STU* all_students = head;
    char name[NAME_LENGTH] = "";
    gotoxy(4, 4);
    printf("请输入要查询的姓名：");
    if (!read_utf8_line(name, sizeof(name)))
    {
        printf("\n姓名读取失败\n");
        return;
    }

    int found = 0;
    while (head != NULL)
    {
        if (strcmp(head->name, name) == 0)
        {
            found = 1;
            printf("\n学号：%d  姓名：%s  排名：第 %d 名\n",
                head->num,
                head->name,
                student_rank(all_students, head));
            printf("语文：%.2f  数学：%.2f  英语：%.2f\n",
                head->score[0], head->score[1], head->score[2]);
            printf("总分：%.2f  平均分：%.2f\n",
                head->total, head->average);
        }
        head = head->next;
    }

    if (!found)
    {
        printf("\n未找到姓名 %s\n", name);
    }
}

// 显示管理菜单
static void print_student_menu(STU* head)
{
    system("cls");
    paintWin(2, 1, 92, 24);
    gotoxy(38, 2);
    printf("学生成绩管理系统");
    gotoxy(61, 2);
    printf("当前用户：%s", current_user);

    gotoxy(5, 4);  printf("1. 录入学生信息");
    gotoxy(5, 6);  printf("2. 各门课程总分和平均分");
    gotoxy(5, 8);  printf("3. 每个学生总分和平均分");
    gotoxy(5, 10); printf("4. 按总分从高到低排序");
    gotoxy(5, 12); printf("5. 按总分从低到高排序");
    gotoxy(5, 14); printf("6. 按学号升序排列");
    gotoxy(5, 16); printf("7. 按姓名排序");

    gotoxy(52, 4);  printf("8. 按学号查询排名和成绩");
    gotoxy(52, 6);  printf("9. 按姓名查询排名和成绩");
    gotoxy(52, 8);  printf("10. 课程等级人数和百分比");
    gotoxy(52, 10); printf("11. 输出全部成绩和课程汇总");
    gotoxy(52, 12); printf("12. 同步记录到数据库");
    gotoxy(52, 14); printf("13. 从数据库读取并显示");
    gotoxy(52, 16); printf("0. 退出登录");

    gotoxy(5, 19);
    printf("学生记录：%d 条", student_count(head));
    gotoxy(5, 21);
    printf("请输入功能编号：");
}

// 处理管理菜单
static STU* student_menu(STU* head)
{
    while (1)
    {
        print_student_menu(head);

        int choice = -1;
        if (scanf("%d", &choice) != 1)
        {
            clear_input_line();
            continue;
        }
        clear_input_line();

        if (choice == 0)
        {
            break;
        }

        system("cls");
        switch (choice)
        {
        case 1:
            head = input_student(head);
            break;
        case 2:
            print_course_summaries(head);
            break;
        case 3:
            print_student_summaries(head);
            break;
        case 4:
            print_rank_table(head, 1);
            break;
        case 5:
            print_rank_table(head, 0);
            break;
        case 6:
            print_sorted_by_id(head);
            break;
        case 7:
            print_sorted_by_name(head);
            break;
        case 8:
            query_by_num(head);
            break;
        case 9:
            query_by_name(head);
            break;
        case 10:
            print_course_grade_statistics(head);
            break;
        case 11:
            print_all_with_summaries(head);
            break;
        case 12:
            if (save_students(head))
            {
                printf("学生记录已同步到MySQL\n");
            }
            break;
        case 13:
            head = load_students(head);
            print_student_table(head);
            break;
        default:
            printf("没有这个功能编号\n");
            break;
        }

        wait_for_key();
    }
    return head;
}

// 程序入口
int main(void)
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    setlocale(LC_ALL, ".UTF-8");

    STU* head = NULL;
    system("cls");
    if (!db_connect())
    {
        printf("\n按任意键退出...");
        _getch();
        return 1;
    }
    head = load_students(head);
    Sleep(700);

    while (1)
    {
        system("cls");
        paintWin(25, 1, 60, 20);
        gotoxy(47, 4);
        printf("===== 学生成绩管理系统 =====");
        gotoxy(40, 8);
        printf("1. 登录系统");
        gotoxy(40, 11);
        printf("2. 注册账号");
        gotoxy(40, 14);
        printf("0. 退出程序");
        gotoxy(38, 17);
        printf("请选择：");

        int choice = -1;
        if (scanf("%d", &choice) != 1)
        {
            clear_input_line();
            continue;
        }
        clear_input_line();

        if (choice == 1)
        {
            if (login_screen())
            {
                head = student_menu(head);
            }
        }
        else if (choice == 2)
        {
            register_screen();
        }
        else if (choice == 0)
        {
            break;
        }
    }

    head = free_students(head);
    db_close();
    return 0;
}
