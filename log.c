#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h> // mkdir,rmdir用

#define LOG_FILE "log.txt"
#define MAX_LINE 512
#define MAX_RECORDS 1024

// ログ構造体
typedef struct {
    char timestamp[32];
    char action[16];     // "move", "mkdir", "rmdir"
    char src[256];
    char dst[256];
} LogRecord;

// 現在時刻を文字列として取得する
static void get_timestamp(char* buf, size_t size) {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// ログ書き込み
void log_write(const char* action, const char* src, const char* dst) {
    FILE* fp = fopen(LOG_FILE, "a");
    if (!fp) {
        printf("ログファイルを開けませんでした\n");
        return;
    }

    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));

    fprintf(fp, "%s | %s | %s | %s\n",
            timestamp, action, src, dst);

    fclose(fp);
}

// ログ表示
void log_print(void) {
    FILE* fp = fopen(LOG_FILE, "r");
    if (!fp) {
        printf("ログがありません\n");
        return;
    }

    char line[MAX_LINE];
    int index = 1;

    while (fgets(line, sizeof(line), fp)) {
        printf("[%d] %s", index, line);
        index++;
    }

    fclose(fp);
}

// ログ読み込み（内部用）
static int load_logs(LogRecord* records) {
    FILE* fp = fopen(LOG_FILE, "r");
    if (!fp) return 0;

    char line[MAX_LINE];
    int count = 0;

    while (fgets(line, sizeof(line), fp) && count < MAX_RECORDS) {
        sscanf(line, "%31[^|] | %15[^|] | %255[^|] | %255[^\n]",
               records[count].timestamp,
               records[count].action,
               records[count].src,
               records[count].dst);
        count++;
    }

    fclose(fp);
    return count;
}

// ログ保存（内部用）
static void save_logs(LogRecord* records, int count) {
    FILE* fp = fopen(LOG_FILE, "w");
    if (!fp) return;

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s | %s | %s | %s\n",
                records[i].timestamp,
                records[i].action,
                records[i].src,
                records[i].dst);
    }

    fclose(fp);
}

// 指定件数戻す（log_back）
void log_back(int count) {
    LogRecord records[MAX_RECORDS];
    int total = load_logs(records);

    if (total == 0) {
        printf("戻すログがありません\n");
        return;
    }

    if (count > total) count = total;

    // 逆順に処理
    for (int i = total - 1; i >= total - count; i--) {
        printf("戻し処理: %s | %s → %s\n",
               records[i].action,
               records[i].dst,
               records[i].src);

        // 戻し処理を記入
    }

    // ログ削除
    save_logs(records, total - count);

    printf("%d 件のログを戻しました\n", count);
}