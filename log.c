#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log.h"
#include "file_org.h"

#define LOG_FILE "log.txt"
#define MAX_LINE 512
#define MAX_RECORDS 1024

/*==================================================
  ログ構造体
==================================================*/
typedef struct {
    char timestamp[32];
    char action[16];
    char src[256];
    char dst[256];
} LogRecord;

/*==================================================
  ログ書き込みルール

  timestamp|action|src|dst

  例:
  2026-05-26 12:00:00|move|A.txt|B.txt

  ※ 区切り文字 "|" を含む名前は禁止
==================================================*/

/*==================================================
  ログ書き込み
  成功:0 失敗:1
==================================================*/
int log_write(const char* action,const char* src,const char* dst) {

    if (!action || !src || !dst) return 1;

    FILE* fp = fopen(LOG_FILE,"a");
    if (!fp) return 1;

    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);

    if (!tm_info) {
        fclose(fp);
        return 1;
    }

    char timestamp[32];

    if (strftime(timestamp,sizeof(timestamp),"%Y-%m-%d %H:%M:%S",tm_info) == 0) {

        fclose(fp);
        return 1;
    }

    if (fprintf(fp,"%s|%s|%s|%s\n",timestamp,action,src,dst) < 0) {

        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}

/*==================================================
  ログ表示
  成功:0 失敗:1
==================================================*/
int log_print(void) {

    FILE* fp = fopen(LOG_FILE,"r");
    if (!fp) return 1;

    char line[MAX_LINE];

    while (fgets(line,sizeof(line),fp)) {
        printf("%s",line);
    }

    fclose(fp);
    return 0;
}
