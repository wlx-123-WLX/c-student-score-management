#ifndef _TEST_H_
#define _TEST_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct user_t {
    char name[20];  // 登录用户名
    char pwd[20];   // 登录密码
} USER_T;

// 注册
int register_user(USER_T user);

// 检查用户名是否已存在
int user_exists(const char* name);

// 登录验证
int login_user(USER_T user);

// 修改账号和密码
int update_user_credentials(const char* old_name, const char* old_pwd,
    const char* new_name, const char* new_pwd);

#endif
