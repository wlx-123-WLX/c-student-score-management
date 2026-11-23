#include "link.h"

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

static int score_in_range(float score)
{
    return score >= 0.0f && score <= 100.0f;
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

static void print_table_header(void)
{
    printf("\n%-8s %-12s %-8s %-8s %-8s %-10s %-10s\n",
        "学号", "姓名", COURSE_NAMES[0], COURSE_NAMES[1],
        COURSE_NAMES[2], "总分", "平均分");
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
        printf("%-8d %-12s %-8.2f %-8.2f %-8.2f %-10.2f %-10.2f\n",
            head->num,
            head->name,
            head->score[0],
            head->score[1],
            head->score[2],
            head->total,
            head->average);
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

    printf("\n%-8s %-12s %-10s %-10s\n", "学号", "姓名", "总分", "平均分");
    while (head != NULL)
    {
        printf("%-8d %-12s %-10.2f %-10.2f\n",
            head->num, head->name, head->total, head->average);
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

        printf("%-8s 总分：%-10.2f 平均分：%-10.2f\n",
            COURSE_NAMES[course], total, total / count);
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
    printf("%-8s %-8s %-12s %-10s\n", "排名", "学号", "姓名", "总分");
    for (int i = 0; i < count; i++)
    {
        printf("%-8d %-8d %-12s %-10.2f\n",
            i + 1, students[i]->num, students[i]->name, students[i]->total);
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
        printf("%-8d %-12s %-8.2f %-8.2f %-8.2f %-10.2f %-10.2f\n",
            students[i]->num,
            students[i]->name,
            students[i]->score[0],
            students[i]->score[1],
            students[i]->score[2],
            students[i]->total,
            students[i]->average);
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
        printf("%-8d %-12s %-8.2f %-8.2f %-8.2f %-10.2f %-10.2f\n",
            students[i]->num,
            students[i]->name,
            students[i]->score[0],
            students[i]->score[1],
            students[i]->score[2],
            students[i]->total,
            students[i]->average);
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
            printf("%-16s 人数：%-4d 百分比：%.2f%%\n",
                labels[grade],
                counts[grade],
                counts[grade] * 100.0f / count);
        }
    }
}

void print_all_with_summaries(STU* head)
{
    print_student_table(head);
    print_course_summaries(head);
}

// 保存到文件
int save_students(STU* head)
{
    FILE* file = fopen(STUDENT_FILE, "w");
    if (file == NULL)
    {
        printf("无法打开 %s\n", STUDENT_FILE);
        return 0;
    }

    // 文件格式：学号|姓名|语文|数学|英语
    while (head != NULL)
    {
        fprintf(file, "%d|%s|%.2f|%.2f|%.2f\n",
            head->num,
            head->name,
            head->score[0],
            head->score[1],
            head->score[2]);
        head = head->next;
    }

    fclose(file);
    printf("记录已保存到 %s\n", STUDENT_FILE);
    return 1;
}

// 从文件读取
STU* load_students(STU* head)
{
    FILE* file = fopen(STUDENT_FILE, "r");
    if (file == NULL)
    {
        printf("未找到 %s，当前没有可读取的记录\n", STUDENT_FILE);
        return head;
    }

    char line[256];
    int loaded = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        STU student = { 0 };
        int matched = sscanf(line, "%d|%19[^|]|%f|%f|%f",
            &student.num,
            student.name,
            &student.score[0],
            &student.score[1],
            &student.score[2]);

        if (matched != 5 || student.num <= 0 ||
            !score_in_range(student.score[0]) ||
            !score_in_range(student.score[1]) ||
            !score_in_range(student.score[2]))
        {
            continue;
        }

        head = add_student(head, student);
        loaded++;
    }

    fclose(file);
    printf("已从 %s 读取 %d 条学生记录\n", STUDENT_FILE, loaded);
    return head;
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
