#include "link.h"

#include "database.h"
#include <wchar.h>
#include <windows.h>

// 课程名称
const char* const COURSE_NAMES[COURSE_COUNT] = {
    "语文",
    "数学",
    "英语"
};

// 安全复制字符串
static void copy_text(char* destination, size_t size, const char* source)
{
    if (size == 0)
    {
        return;
    }

    if (source == NULL)
    {
        source = "";
    }

    strncpy(destination, source, size - 1);
    destination[size - 1] = '\0';
}

// 计算总分和平均分
void calculate_student_summary(STU* student)
{
    if (student == NULL)
    {
        return;
    }

    student->total = 0.0f;
    for (int i = 0; i < COURSE_COUNT; i++)
    {
        student->total += student->score[i];
    }
    student->average = student->total / COURSE_COUNT;
}

// 添加学生
STU* add_student(STU* head, STU student)
{
    STU* node = (STU*)calloc(1, sizeof(STU));
    if (node == NULL)
    {
        printf("内存分配失败\n");
        return head;
    }

    *node = student;
    node->next = NULL;
    calculate_student_summary(node);

    if (head == NULL)
    {
        return node;
    }

    STU* current = head;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = node;
    return head;
}

STU* find_student_by_num(STU* head, int num)
{
    while (head != NULL)
    {
        if (head->num == num)
        {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

STU* find_student_by_name(STU* head, const char* name)
{
    while (head != NULL)
    {
        if (strcmp(head->name, name) == 0)
        {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

int student_count(STU* head)
{
    int count = 0;
    while (head != NULL)
    {
        count++;
        head = head->next;
    }
    return count;
}

// 计算排名
int student_rank(STU* head, const STU* target)
{
    if (target == NULL)
    {
        return 0;
    }

    int rank = 1;
    while (head != NULL)
    {
        if (head->total > target->total)
        {
            rank++;
        }
        head = head->next;
    }
    return rank;
}

enum
{
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
};

static int is_wide_character(wchar_t ch)
{
    return
        (ch >= 0x1100 && ch <= 0x115F) ||
        (ch >= 0x2E80 && ch <= 0xA4CF) ||
        (ch >= 0xAC00 && ch <= 0xD7A3) ||
        (ch >= 0xF900 && ch <= 0xFAFF) ||
        (ch >= 0xFE10 && ch <= 0xFE19) ||
        (ch >= 0xFE30 && ch <= 0xFE6F) ||
        (ch >= 0xFF00 && ch <= 0xFF60) ||
        (ch >= 0xFFE0 && ch <= 0xFFE6);
}

static int utf8_display_width(const char* text)
{
    int wide_length = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    if (wide_length <= 0)
    {
        return (int)strlen(text);
    }

    wchar_t* wide_text = (wchar_t*)malloc(sizeof(wchar_t) * wide_length);
    if (wide_text == NULL)
    {
        return (int)strlen(text);
    }
    MultiByteToWideChar(CP_UTF8, 0, text, -1, wide_text, wide_length);

    int width = 0;
    for (int i = 0; i < wide_length && wide_text[i] != L'\0'; i++)
    {
        width += is_wide_character(wide_text[i]) ? 2 : 1;
    }
    free(wide_text);
    return width;
}

static void print_utf8_cell(const char* text, int width, int alignment)
{
    int content_width = utf8_display_width(text);
    int padding = width - content_width;
    if (padding < 0)
    {
        padding = 0;
    }

    int left_padding = 0;
    int right_padding = padding;
    if (alignment == ALIGN_RIGHT)
    {
        left_padding = padding;
        right_padding = 0;
    }
    else if (alignment == ALIGN_CENTER)
    {
        left_padding = padding / 2;
        right_padding = padding - left_padding;
    }

    printf("%*s%s%*s", left_padding, "", text, right_padding, "");
}

static void print_number_cell(int number, int width)
{
    char text[32];
    snprintf(text, sizeof(text), "%d", number);
    print_utf8_cell(text, width, ALIGN_RIGHT);
}

static void print_float_cell(float number, int width)
{
    char text[32];
    snprintf(text, sizeof(text), "%.2f", number);
    print_utf8_cell(text, width, ALIGN_RIGHT);
}

static void print_table_header(void)
{
    putchar('\n');
    print_utf8_cell("学号", 8, ALIGN_RIGHT);
    putchar(' ');
    print_utf8_cell("姓名", 10, ALIGN_LEFT);
    putchar(' ');
    print_utf8_cell(COURSE_NAMES[0], 8, ALIGN_RIGHT);
    putchar(' ');
    print_utf8_cell(COURSE_NAMES[1], 8, ALIGN_RIGHT);
    putchar(' ');
    print_utf8_cell(COURSE_NAMES[2], 8, ALIGN_RIGHT);
    putchar(' ');
    print_utf8_cell("总分", 10, ALIGN_RIGHT);
    putchar(' ');
    print_utf8_cell("平均分", 10, ALIGN_RIGHT);
    putchar('\n');
}

static void print_full_student_row(const STU* student)
{
    print_number_cell(student->num, 8);
    putchar(' ');
    print_utf8_cell(student->name, 10, ALIGN_LEFT);
    putchar(' ');
    print_float_cell(student->score[0], 8);
    putchar(' ');
    print_float_cell(student->score[1], 8);
    putchar(' ');
    print_float_cell(student->score[2], 8);
    putchar(' ');
    print_float_cell(student->total, 10);
    putchar(' ');
    print_float_cell(student->average, 10);
    putchar('\n');
}

void print_student_table(STU* head)
{
    if (head == NULL)
    {
        printf("暂无学生记录\n");
        return;
    }

    print_table_header();
    while (head != NULL)
    {
        print_full_student_row(head);
        head = head->next;
    }
}

void print_student_summaries(STU* head)
{
    if (head == NULL)
    {
        printf("暂无学生记录\n");
        return;
    }

    putchar('\n');
    print_utf8_cell("学号", 8, ALIGN_RIGHT);
    putchar(' ');
    print_utf8_cell("姓名", 10, ALIGN_LEFT);
    putchar(' ');
    print_utf8_cell("总分", 10, ALIGN_RIGHT);
    putchar(' ');
    print_utf8_cell("平均分", 10, ALIGN_RIGHT);
    putchar('\n');
    while (head != NULL)
    {
        print_number_cell(head->num, 8);
        putchar(' ');
        print_utf8_cell(head->name, 10, ALIGN_LEFT);
        putchar(' ');
        print_float_cell(head->total, 10);
        putchar(' ');
        print_float_cell(head->average, 10);
        putchar('\n');
        head = head->next;
    }
}

// 输出课程汇总
void print_course_summaries(STU* head)
{
    int count = student_count(head);
    if (count == 0)
    {
        printf("暂无学生记录，无法统计课程成绩\n");
        return;
    }

    printf("\n课程统计：\n");
    for (int course = 0; course < COURSE_COUNT; course++)
    {
        float total = 0.0f;
        STU* current = head;

        while (current != NULL)
        {
            total += current->score[course];
            current = current->next;
        }

        print_utf8_cell(COURSE_NAMES[course], 8, ALIGN_LEFT);
        printf(" 总分：");
        print_float_cell(total, 10);
        printf(" 平均分：");
        print_float_cell(total / count, 10);
        putchar('\n');
    }
}

// 创建指针数组
static STU** create_pointer_array(STU* head, int* count)
{
    *count = student_count(head);
    if (*count == 0)
    {
        return NULL;
    }

    STU** students = (STU**)malloc(sizeof(STU*) * (*count));
    if (students == NULL)
    {
        printf("内存分配失败\n");
        *count = 0;
        return NULL;
    }

    int index = 0;
    while (head != NULL)
    {
        students[index++] = head;
        head = head->next;
    }
    return students;
}

static int compare_total_descending(const void* left, const void* right)
{
    const STU* a = *(const STU* const*)left;
    const STU* b = *(const STU* const*)right;
    if (a->total < b->total) return 1;
    if (a->total > b->total) return -1;
    if (a->num < b->num) return -1;
    if (a->num > b->num) return 1;
    return 0;
}

static int compare_total_ascending(const void* left, const void* right)
{
    const STU* a = *(const STU* const*)left;
    const STU* b = *(const STU* const*)right;
    if (a->total > b->total) return 1;
    if (a->total < b->total) return -1;
    if (a->num < b->num) return -1;
    if (a->num > b->num) return 1;
    return 0;
}

static int compare_num(const void* left, const void* right)
{
    const STU* a = *(const STU* const*)left;
    const STU* b = *(const STU* const*)right;
    if (a->num < b->num) return -1;
    if (a->num > b->num) return 1;
    return 0;
}

static int compare_name(const void* left, const void* right)
{
    const STU* a = *(const STU* const*)left;
    const STU* b = *(const STU* const*)right;
    return strcmp(a->name, b->name);
}

// 输出排名表
void print_rank_table(STU* head, int descending)
{
    int count = 0;
    STU** students = create_pointer_array(head, &count);
    if (students == NULL)
    {
        printf("暂无学生记录\n");
        return;
    }

    qsort(students, count,
        sizeof(STU*),
        descending ? compare_total_descending : compare_total_ascending);

    printf("\n%s总分名次表：\n", descending ? "由高到低" : "由低到高");
    print_utf8_cell("排名", 8, ALIGN_RIGHT);
    putchar(' ');
    print_utf8_cell("学号", 8, ALIGN_RIGHT);
    putchar(' ');
    print_utf8_cell("姓名", 10, ALIGN_LEFT);
    putchar(' ');
    print_utf8_cell("总分", 10, ALIGN_RIGHT);
    putchar('\n');
    for (int i = 0; i < count; i++)
    {
        print_number_cell(i + 1, 8);
        putchar(' ');
        print_number_cell(students[i]->num, 8);
        putchar(' ');
        print_utf8_cell(students[i]->name, 10, ALIGN_LEFT);
        putchar(' ');
        print_float_cell(students[i]->total, 10);
        putchar('\n');
    }

    free(students);
}

void print_sorted_by_id(STU* head)
{
    int count = 0;
    STU** students = create_pointer_array(head, &count);
    if (students == NULL)
    {
        printf("暂无学生记录\n");
        return;
    }

    qsort(students, count, sizeof(STU*), compare_num);
    printf("\n按学号升序：\n");
    print_table_header();
    for (int i = 0; i < count; i++)
    {
        print_full_student_row(students[i]);
    }
    free(students);
}

void print_sorted_by_name(STU* head)
{
    int count = 0;
    STU** students = create_pointer_array(head, &count);
    if (students == NULL)
    {
        printf("暂无学生记录\n");
        return;
    }

    qsort(students, count, sizeof(STU*), compare_name);
    printf("\n按姓名排序：\n");
    print_table_header();
    for (int i = 0; i < count; i++)
    {
        print_full_student_row(students[i]);
    }
    free(students);
}

// 成绩等级
static int grade_category(float score)
{
    if (score >= 90.0f) return 0;
    if (score >= 80.0f) return 1;
    if (score >= 70.0f) return 2;
    if (score >= 60.0f) return 3;
    return 4;
}

// 等级统计
void print_course_grade_statistics(STU* head)
{
    int count = student_count(head);
    if (count == 0)
    {
        printf("暂无学生记录，无法统计等级\n");
        return;
    }

    const char* labels[] = {
        "优秀(90-100)",
        "良好(80-89)",
        "中等(70-79)",
        "及格(60-69)",
        "不及格(0-59)"
    };

    for (int course = 0; course < COURSE_COUNT; course++)
    {
        int counts[5] = { 0 };
        STU* current = head;

        while (current != NULL)
        {
            counts[grade_category(current->score[course])]++;
            current = current->next;
        }

        printf("\n%s等级统计：\n", COURSE_NAMES[course]);
        for (int grade = 0; grade < 5; grade++)
        {
            print_utf8_cell(labels[grade], 20, ALIGN_LEFT);
            printf(" 人数：");
            print_number_cell(counts[grade], 4);
            printf(" 百分比：");
            print_float_cell(counts[grade] * 100.0f / count, 7);
            printf("%%\n");
        }
    }
}

void print_all_with_summaries(STU* head)
{
    print_student_table(head);
    print_course_summaries(head);
}

// 保存到数据库
int save_students(STU* head)
{
    return db_sync_students(head);
}

// 从数据库读取
STU* load_students(STU* head)
{
    head = free_students(head);
    return db_load_students();
}

// 释放链表
STU* free_students(STU* head)
{
    while (head != NULL)
    {
        STU* next = head->next;
        free(head);
        head = next;
    }
    return NULL;
}
