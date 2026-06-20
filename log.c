#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log.h"

#define LOG_FILE "log.txt"

void log_write(const char *action,const char *src,const char *dst)
{
    FILE *fp;

    /* 追記モードで開く */
    fp = fopen(LOG_FILE, "a");

    // ファイルが開けない場合
    if(fp == NULL)
    {
        printf("エラー：ログファイルを開けませんでした。action=%s, src=%s, dst=%s\n",action, src, dst);
        return;
    }

    /* 現在時刻取得 */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // 現在時刻を取得できない場合
    if(t == NULL)
    {
        printf("エラー：時刻取得に失敗しました。action=%s, src=%s, dst=%s\n",action, src, dst);
        fclose(fp);
        return;
    }

    /* ログ出力 */
    if (fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d|%s|%s|%s\n",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,t->tm_hour, t->tm_min, t->tm_sec,action, src, dst) < 0)
    {
        printf("エラー：ログ書き込みに失敗しました。action=%s, src=%s, dst=%s\n",action, src, dst);
    }


    fclose(fp);
}

void log_print(void)
{
    FILE *fp;

    fp = fopen(LOG_FILE, "r");

    if(fp == NULL)
    {
        printf("エラー：ログファイルが存在しません。\n");
        return;
    }

    char line[1024];

    printf("\n");
    printf("========== ログ ==========\n");

    while(fgets(line, sizeof(line), fp) != NULL)
    {
        printf("%s", line);
    }
    printf("=========================\n");

    fclose(fp);
}