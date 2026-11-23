#include "test.h"

#include <ctype.h>
#include <windows.h>

// 解析账号
static int parse_user_line(char* line, char** name, char** password)
{
    *name = line;
    while (**name != '\0' && isspace((unsigned char)**name))
    {
        (*name)++;
    }

    char* separator = *name;
    while (*separator != '\0' && *separator != ',' &&
        !isspace((unsigned char)*separator))
    {
        separator++;
    }
    if (*separator == '\0')
    {
        return 0;
    }

    *separator++ = '\0';
    while (*separator == ',' || isspace((unsigned char)*separator))
    {
        separator++;
    }

    *password = separator;
    while (*separator != '\0' && *separator != ',' &&
        !isspace((unsigned char)*separator))
    {
        separator++;
    }
    *separator = '\0';

    return **name != '\0' && **password != '\0';
}

// 检查用户名
int user_exists(const char* name)
{
    FILE* fp = fopen("user.txt", "r");
    if (fp == NULL)
    {
        return 0;
    }

    char line[128];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char* stored_name = NULL;
        char* stored_password = NULL;
        if (parse_user_line(line, &stored_name, &stored_password) &&
            strcmp(stored_name, name) == 0)
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

// 注册
int register_user(USER_T user)
{
    if (user_exists(user.name))
    {
        return 0;
    }

    FILE* fp = fopen("user.txt", "a");
    if (fp == NULL)
    {
        return 0;
    }

    fprintf(fp, "%s,%s\n", user.name, user.pwd);
    fclose(fp);
    return 1;
}

// 登录验证
int login_user(USER_T user)
{
    FILE* fp = fopen("user.txt", "r");
    if (fp == NULL)
    {
        return 0;
    }

    char line[128];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char* name = NULL;
        char* password = NULL;
        if (parse_user_line(line, &name, &password) &&
            strcmp(name, user.name) == 0 &&
            strcmp(password, user.pwd) == 0)
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

// 修改账号和密码
int update_user_credentials(const char* old_name, const char* old_pwd,
    const char* new_name, const char* new_pwd)
{
    FILE* input = fopen("user.txt", "r");
    if (input == NULL)
    {
        return 0;
    }

    FILE* output = fopen("user.tmp", "w");
    if (output == NULL)
    {
        fclose(input);
        return 0;
    }

    int replaced = 0;
    char line[128];
    while (fgets(line, sizeof(line), input) != NULL)
    {
        char original[128];
        strcpy(original, line);

        char* name = NULL;
        char* password = NULL;
        if (!parse_user_line(line, &name, &password))
        {
            fputs(original, output);
            continue;
        }

        if (strcmp(name, old_name) == 0 &&
            strcmp(password, old_pwd) == 0)
        {
            fprintf(output, "%s,%s\n", new_name, new_pwd);
            replaced = 1;
        }
        else
        {
            fprintf(output, "%s,%s\n", name, password);
        }
    }

    fclose(input);
    fclose(output);

    if (!replaced)
    {
        remove("user.tmp");
        return 0;
    }

    if (!MoveFileExA("user.tmp", "user.txt",
        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
    {
        remove("user.tmp");
        return 0;
    }

    return 1;
}
