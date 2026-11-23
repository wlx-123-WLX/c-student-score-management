# C 语言学生成绩管理系统

这是一个 Visual Studio 2022 的 C 语言控制台项目，使用链表处理学生数据，
并使用 MySQL 持久化学生成绩和登录账号。

## 项目结构

- `main.c`：登录、注册、菜单和功能入口
- `link.c`：学生链表、统计、排序和查询
- `link.h`：学生结构体和函数声明
- `database.c`：MySQL 连接和数据库操作
- `test.c`：账号注册、登录和修改
- `database_setup.sql`：数据库表结构

## 打开方式

1. 双击 `1121.1.sln`。
2. 选择 `Debug | x64`。
3. 按 `F5` 构建并运行。

编译环境：Visual Studio 2022，并安装“使用 C++ 的桌面开发”工作负载。

运行前还需要安装 MySQL Server 8.0。

## 数据库配置

1. 使用 Navicat 连接本机 MySQL。
2. 执行 `database_setup.sql` 创建数据库和数据表。
3. 复制 `db_config.example.h` 为 `db_config.h`。
4. 修改数据库地址、用户名和数据库名。

如果 `DB_PASSWORD` 留空，程序启动时会提示输入 MySQL 密码。

`db_config.h` 已加入 `.gitignore`，不要提交真实数据库密码。

## 登录与注册

项目启动后可以先注册账号，再用账号登录。

账号保存在 MySQL 的 `users` 表中，密码以 SHA-256 哈希形式保存。

## 已实现功能

1. 录入学号、姓名、语文、数学、英语成绩
2. 统计每门课程的总分和平均分
3. 统计每个学生的总分和平均分
4. 按总分从高到低排名
5. 按总分从低到高排名
6. 按学号升序排列
7. 按姓名排序
8. 按学号查询排名和成绩
9. 按姓名查询排名和成绩
10. 按优秀、良好、中等、及格、不及格统计课程人数和百分比
11. 输出全部成绩和课程汇总
12. 将学生记录同步到 MySQL
13. 从 MySQL 读取并显示

## 数据文件

学生数据保存在 `students` 表中：

```sql
student_no, name, chinese, math, english
```

旧版 `students.txt` 只作为迁移备份保留，程序不再直接读写该文件。

## 修改科目

当前科目数量由 `link.h` 中的 `COURSE_COUNT` 控制，科目名称在
`link.c` 的 `COURSE_NAMES` 中定义。

如果增加科目，需要同步修改：

1. `COURSE_COUNT`
2. `COURSE_NAMES`
3. `database.c` 中 MySQL 保存和读取的字段
4. `main.c` 中学生录入的输入项
5. 输出表格的表头

## 说明

本项目主要用于 C 语言课程学习和 GitHub 开源展示。

本项目主要用于学习和课程展示。虽然账号密码使用 SHA-256 保存，
生产环境仍应使用带盐密码哈希和更完整的权限控制。

## License

本项目使用 [MIT License](LICENSE)。
