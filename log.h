#ifndef LOG_H
#define LOG_H

// ログ1件分の構造体
typedef struct {
    char timestamp[32];
    char action[16];     // "move", "mkdir", "rmdir"
    char src[256];
    char dst[256];
} LogRecord;

// ログ書き込み
void log_write(const char* action, const char* src, const char* dst);

// ログ表示
void log_print(void);

// 指定件数戻す（戻した分のログは削除）
void log_back(int count);

#endif