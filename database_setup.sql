CREATE DATABASE IF NOT EXISTS student_score_db
DEFAULT CHARACTER SET utf8mb4
COLLATE utf8mb4_0900_ai_ci;

USE student_score_db;

CREATE TABLE IF NOT EXISTS students (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    student_no INT NOT NULL UNIQUE,
    name VARCHAR(50) NOT NULL,
    chinese DECIMAL(5,2) NOT NULL,
    math DECIMAL(5,2) NOT NULL,
    english DECIMAL(5,2) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT chk_chinese CHECK (chinese BETWEEN 0 AND 100),
    CONSTRAINT chk_math CHECK (math BETWEEN 0 AND 100),
    CONSTRAINT chk_english CHECK (english BETWEEN 0 AND 100)
);

CREATE TABLE IF NOT EXISTS users (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password_hash CHAR(64) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

INSERT INTO students (student_no, name, chinese, math, english)
VALUES
    (1001, '张三', 90, 85, 88),
    (1002, '李四', 78, 92, 80),
    (1003, '王五', 65, 70, 75)
ON DUPLICATE KEY UPDATE
    name = VALUES(name),
    chinese = VALUES(chinese),
    math = VALUES(math),
    english = VALUES(english);
