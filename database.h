#ifndef DATABASE_H
#define DATABASE_H

#include "link.h"

int db_connect(void);
void db_close(void);

STU* db_load_students(void);
int db_sync_students(STU* head);

int db_user_exists(const char* username);
int db_register_user(const char* username, const char* password);
int db_login_user(const char* username, const char* password);
int db_update_user(const char* old_name, const char* old_password,
    const char* new_name, const char* new_password);

#endif
