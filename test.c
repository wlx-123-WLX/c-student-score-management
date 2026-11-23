#include "test.h"
#include "database.h"

// 检查用户名
int user_exists(const char* name)
{
    return db_user_exists(name);
}

// 注册
int register_user(USER_T user)
{
    return db_register_user(user.name, user.pwd);
}

// 登录验证
int login_user(USER_T user)
{
    return db_login_user(user.name, user.pwd);
}

// 修改账号和密码
int update_user_credentials(const char* old_name, const char* old_pwd,
    const char* new_name, const char* new_pwd)
{
    return db_update_user(old_name, old_pwd, new_name, new_pwd);
}
