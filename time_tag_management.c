#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "tag.h"

#define MAX_TAGS 50
#define MAX_FILES 50
#define MAX_LEN 200

// タグ1つのデータ
typedef struct {
    char tag[50];
    char files[MAX_FILES][MAX_LEN];
    int fileCount;
} Tag;

// 全タグ
Tag tags[MAX_TAGS];
int tagCount = 0;

/* ===== 読み込み ===== */
void loadTags() {
    FILE *fp = fopen("tag.txt", "r");
    if (!fp) return;

    char line[512];

    while (fgets(line, sizeof(line), fp)) {

        char *p = strtok(line, " \n");
        if (!p) continue;

        strcpy(tags[tagCount].tag, p);
        tags[tagCount].fileCount = 0;

        while ((p = strtok(NULL, " \n")) != NULL) {
            strcpy(tags[tagCount].files[tags[tagCount].fileCount], p);
            tags[tagCount].fileCount++;
        }

        tagCount++;
    }

    fclose(fp);
}

/* ===== 保存 ===== */
void saveTags() {
    FILE *fp = fopen("tag.txt", "w");
    if (!fp) return;

    for (int i = 0; i < tagCount; i++) {
        fprintf(fp, "%s", tags[i].tag);

        for (int j = 0; j < tags[i].fileCount; j++) {
            fprintf(fp, " %s", tags[i].files[j]);
        }

        fprintf(fp, "\n");
    }

    fclose(fp);
}

/* ===== ファイル選択 ===== */
void selectFile(char *out) {
    DIR *dir = opendir(".");
    struct dirent *d;

    char list[100][256];
    int i = 0;

    printf("files:\n");

    while ((d = readdir(dir)) != NULL) {
        if (d->d_type == DT_REG) {
            printf("%d:%s\n", i, d->d_name);
            strcpy(list[i], d->d_name);
            i++;
        }
    }

    closedir(dir);

    int sel;
    printf("select file: ");
    scanf("%d", &sel);

    if (sel < 0 || sel >= i) return;

    strcpy(out, list[sel]);
}

/* ===== タグ選択 ===== */
void selectTag(char *out) {

    printf("tags:\n");

    for (int i = 0; i < tagCount; i++) {
        printf("%d:%s\n", i, tags[i].tag);
    }

    int sel;
    printf("select tag: ");
    scanf("%d", &sel);

    if (sel < 0 || sel >= tagCount) return;

    strcpy(out, tags[sel].tag);
}

/* ===== タグ作成 ===== */
void createTag(char *tag, char *file) {
    strcpy(tags[tagCount].tag, tag);
    strcpy(tags[tagCount].files[0], file);
    tags[tagCount].fileCount = 1;
    tagCount++;
}

/* ===== ファイル追加 ===== */
void addFile(char *tag, char *file) {

    for (int i = 0; i < tagCount; i++) {
        if (strcmp(tags[i].tag, tag) == 0) {
            strcpy(tags[i].files[tags[i].fileCount], file);
            tags[i].fileCount++;
            return;
        }
    }

    createTag(tag, file);
}

/* ===== タグ削除 ===== */
void deleteTag(char *tag) {

    for (int i = 0; i < tagCount; i++) {
        if (strcmp(tags[i].tag, tag) == 0) {

            for (int j = i; j < tagCount - 1; j++) {
                tags[j] = tags[j + 1];
            }

            tagCount--;
            return;
        }
    }
}

/* ===== ファイル削除 ===== */
void removeFile(char *tag, char *file) {

    for (int i = 0; i < tagCount; i++) {
        if (strcmp(tags[i].tag, tag) == 0) {

            for (int j = 0; j < tags[i].fileCount; j++) {
                if (strcmp(tags[i].files[j], file) == 0) {

                    for (int k = j; k < tags[i].fileCount - 1; k++) {
                        strcpy(tags[i].files[k], tags[i].files[k + 1]);
                    }

                    tags[i].fileCount--;
                    return;
                }
            }
        }
    }
}

/* ===== 一覧表示 ===== */
void listTags() {
    for (int i = 0; i < tagCount; i++) {
        printf("%d:%s\n", i, tags[i].tag);
    }
}