#ifndef LINK_H
#define LINK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COURSE_COUNT 3               // 当前课程数量：语文、数学、英语
#define NAME_LENGTH 20               // 学生姓名最多保存 19 个字符

// 课程名称数组，下标与 STU.score 一一对应
extern const char* const COURSE_NAMES[COURSE_COUNT];

typedef struct student
{
    int num;                       // 学号
    char name[NAME_LENGTH];        // 姓名
    float score[COURSE_COUNT];     // 各科成绩
    float total;                   // 个人总分
    float average;                 // 个人平均分
    struct student* next;          // 指向下一个学生的指针
} STU;

// 计算学生总分和平均分
void calculate_student_summary(STU* student);

// 向链表尾部添加一个学生
STU* add_student(STU* head, STU student);

// 按学号或姓名查找学生
STU* find_student_by_num(STU* head, int num);
STU* find_student_by_name(STU* head, const char* name);

// 获取学生数量和学生排名
int student_count(STU* head);
int student_rank(STU* head, const STU* target);

// 输出学生表格、个人汇总和课程汇总
void print_student_table(STU* head);
void print_student_summaries(STU* head);
void print_course_summaries(STU* head);

// 按总分、学号或姓名输出排序结果
void print_rank_table(STU* head, int descending);
void print_sorted_by_id(STU* head);
void print_sorted_by_name(STU* head);

// 统计各门课程的成绩等级
void print_course_grade_statistics(STU* head);
void print_all_with_summaries(STU* head);

// 学生记录的数据库同步、读取和释放
int save_students(STU* head);
STU* load_students(STU* head);
STU* free_students(STU* head);

#endif
