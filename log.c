#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log.h"

#define LOG_FILE "log.txt"

void log_write(
    const char *action,
    const char *src,
    const char *dst
)
{
    FILE *fp;

    /* 追記モードで開く */
    fp = fopen(LOG_FILE, "a");

    if(fp == NULL)
    {
        return;
    }

    /* 現在時刻取得 */
    time_t now = time(NULL);

    struct tm *t = localtime(&now);

    if(t == NULL)
    {
        fclose(fp);
        return;
    }

    /* ログ出力 */
    fprintf(
        fp,
        "%04d-%02d-%02d %02d:%02d:%02d|%s|%s|%s\n",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec,
        action,
        src,
        dst
    );

    fclose(fp);
}

void log_print(void)
{
    FILE *fp;

    fp = fopen(LOG_FILE, "r");

    if(fp == NULL)
    {
        printf("ログファイルが存在しません。\n");
        return;
    }

    char line[1024];

    printf("\n");
    printf("========== LOG ==========\n");

    while(fgets(line, sizeof(line), fp) != NULL)
    {
        printf("%s", line);
    }

    printf("=========================\n");

    fclose(fp);
}