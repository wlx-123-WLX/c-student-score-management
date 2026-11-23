#include "database.h"
#include "db_config.h"
#include <mysql.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static MYSQL* g_db = NULL;

static void read_db_password(char* password, int size)
{
    int length = 0;
    printf("请输入MySQL密码：");
    fflush(stdout);
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
            putchar('*');
            fflush(stdout);
        }
    }
    password[length] = '\0';
    printf("\n");
    fflush(stdout);
}

int db_connect(void)
{
    char password[64] = "";
    const char* env_password = getenv("STUDENT_DB_PASSWORD");

    if (DB_PASSWORD[0] != '\0')
    {
        strcpy(password, DB_PASSWORD);
    }
    else if (env_password != NULL && env_password[0] != '\0')
    {
        strncpy(password, env_password, sizeof(password) - 1);
        password[sizeof(password) - 1] = '\0';
    }
    else
    {
        read_db_password(password, sizeof(password));
    }

    g_db = mysql_init(NULL);
    if (g_db == NULL)
    {
        printf("MySQL初始化失败\n");
        return 0;
    }

    mysql_options(g_db, MYSQL_SET_CHARSET_NAME, "utf8mb4");
    if (mysql_real_connect(g_db, DB_HOST, DB_USER, password, DB_NAME,
        DB_PORT, NULL, 0) == NULL)
    {
        printf("MySQL连接失败：%s\n", mysql_error(g_db));
        mysql_close(g_db);
        g_db = NULL;
        return 0;
    }

    mysql_set_character_set(g_db, "utf8mb4");
    printf("MySQL连接成功\n");
    return 1;
}

void db_close(void)
{
    if (g_db != NULL)
    {
        mysql_close(g_db);
        g_db = NULL;
    }
}

static int escape_text(const char* source, char* destination, size_t size)
{
    unsigned long length = (unsigned long)strlen(source);
    if (length * 2 + 1 > size)
    {
        return 0;
    }

    mysql_real_escape_string(g_db, destination, source, length);
    return 1;
}

static int run_query(const char* sql)
{
    if (mysql_query(g_db, sql) != 0)
    {
        printf("数据库操作失败：%s\n", mysql_error(g_db));
        return 0;
    }
    return 1;
}

STU* db_load_students(void)
{
    STU* head = NULL;
    const char* sql =
        "SELECT student_no, name, chinese, math, english "
        "FROM students ORDER BY student_no";

    if (!run_query(sql))
    {
        return NULL;
    }

    MYSQL_RES* result = mysql_store_result(g_db);
    if (result == NULL)
    {
        printf("读取结果失败：%s\n", mysql_error(g_db));
        return NULL;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL)
    {
        STU student = { 0 };
        student.num = atoi(row[0]);
        strncpy(student.name, row[1], sizeof(student.name) - 1);
        student.score[0] = (float)atof(row[2]);
        student.score[1] = (float)atof(row[3]);
        student.score[2] = (float)atof(row[4]);
        head = add_student(head, student);
    }

    mysql_free_result(result);
    return head;
}

int db_sync_students(STU* head)
{
    STU* current = head;
    char escaped_name[NAME_LENGTH * 2 + 1];
    char sql[512];

    if (!run_query("START TRANSACTION") ||
        !run_query("DELETE FROM students"))
    {
        return 0;
    }

    while (current != NULL)
    {
        if (!escape_text(current->name, escaped_name, sizeof(escaped_name)))
        {
            run_query("ROLLBACK");
            return 0;
        }

        snprintf(sql, sizeof(sql),
            "INSERT INTO students(student_no,name,chinese,math,english) "
            "VALUES(%d,'%s',%.2f,%.2f,%.2f)",
            current->num,
            escaped_name,
            current->score[0],
            current->score[1],
            current->score[2]);

        if (!run_query(sql))
        {
            run_query("ROLLBACK");
            return 0;
        }
        current = current->next;
    }

    if (!run_query("COMMIT"))
    {
        run_query("ROLLBACK");
        return 0;
    }

    return 1;
}

int db_user_exists(const char* username)
{
    char escaped_name[128];
    char sql[256];

    if (!escape_text(username, escaped_name, sizeof(escaped_name)))
    {
        return 0;
    }

    snprintf(sql, sizeof(sql),
        "SELECT id FROM users WHERE username='%s' LIMIT 1",
        escaped_name);
    if (!run_query(sql))
    {
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db);
    if (result == NULL)
    {
        return 0;
    }

    int exists = mysql_num_rows(result) > 0;
    mysql_free_result(result);
    return exists;
}

int db_register_user(const char* username, const char* password)
{
    char escaped_name[128];
    char escaped_password[256];
    char sql[1024];

    if (db_user_exists(username))
    {
        return 0;
    }

    if (!escape_text(username, escaped_name, sizeof(escaped_name)) ||
        !escape_text(password, escaped_password, sizeof(escaped_password)))
    {
        return 0;
    }

    snprintf(sql, sizeof(sql),
        "INSERT INTO users(username,password_hash) "
        "VALUES('%s',SHA2('%s',256))",
        escaped_name,
        escaped_password);
    return run_query(sql);
}

int db_login_user(const char* username, const char* password)
{
    char escaped_name[128];
    char escaped_password[256];
    char sql[1024];

    if (!escape_text(username, escaped_name, sizeof(escaped_name)) ||
        !escape_text(password, escaped_password, sizeof(escaped_password)))
    {
        return 0;
    }

    snprintf(sql, sizeof(sql),
        "SELECT id FROM users "
        "WHERE username='%s' AND password_hash=SHA2('%s',256) LIMIT 1",
        escaped_name,
        escaped_password);
    if (!run_query(sql))
    {
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db);
    if (result == NULL)
    {
        return 0;
    }

    int matched = mysql_num_rows(result) > 0;
    mysql_free_result(result);
    return matched;
}

int db_update_user(const char* old_name, const char* old_password,
    const char* new_name, const char* new_password)
{
    char escaped_old_name[128];
    char escaped_old_password[256];
    char escaped_new_name[128];
    char escaped_new_password[256];
    char sql[1024];

    if (!escape_text(old_name, escaped_old_name, sizeof(escaped_old_name)) ||
        !escape_text(old_password, escaped_old_password, sizeof(escaped_old_password)) ||
        !escape_text(new_name, escaped_new_name, sizeof(escaped_new_name)) ||
        !escape_text(new_password, escaped_new_password, sizeof(escaped_new_password)))
    {
        return 0;
    }

    snprintf(sql, sizeof(sql),
        "UPDATE users SET username='%s', password_hash=SHA2('%s',256) "
        "WHERE username='%s' AND password_hash=SHA2('%s',256)",
        escaped_new_name,
        escaped_new_password,
        escaped_old_name,
        escaped_old_password);

    if (!run_query(sql))
    {
        return 0;
    }
    return mysql_affected_rows(g_db) > 0;
}
