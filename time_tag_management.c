#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TAGS 50
#define MAX_FILES 50
#define MAX_LEN 200

typedef struct {
    char tag[50];
    char files[MAX_FILES][MAX_LEN];
    int fileCount;
} Tag;

Tag tags[MAX_TAGS];
int tagCount = 0;

void loadTags() {

    FILE *fp = fopen("tag.txt", "r");
    if (fp == NULL) return;

    char line[512];

    while (fgets(line, sizeof(line), fp)) {

        char *token = strtok(line, " \n");
        if (!token) continue;

        strcpy(tags[tagCount].tag, token);
        tags[tagCount].fileCount = 0;

        while ((token = strtok(NULL, " \n")) != NULL) {
            strcpy(tags[tagCount].files[tags[tagCount].fileCount], token);
            tags[tagCount].fileCount++;
        }

        tagCount++;
    }

    fclose(fp);
}

void saveTags() {

    FILE *fp = fopen("tag.txt", "w");

    for (int i = 0; i < tagCount; i++) {

        fprintf(fp, "%s", tags[i].tag);

        for (int j = 0; j < tags[i].fileCount; j++) {
            fprintf(fp, " %s", tags[i].files[j]);
        }

        fprintf(fp, "\n");
    }

    fclose(fp);
}

void addNewTag(char *tagName, char *filePath) {

    strcpy(tags[tagCount].tag, tagName);
    strcpy(tags[tagCount].files[0], filePath);
    tags[tagCount].fileCount = 1;

    tagCount++;
}

void addFileToTag(char *tagName, char *filePath) {

    for (int i = 0; i < tagCount; i++) {

        if (strcmp(tags[i].tag, tagName) == 0) {

            strcpy(tags[i].files[tags[i].fileCount], filePath);
            tags[i].fileCount++;

            return;
        }
    }

    addNewTag(tagName, filePath);
}

void tag(){

    loadTags();

    char tag[50];
    char path[200];

    int mode;

    printf("1:新規タグ 2:追加 : ");
    scanf("%d", &mode);

    printf("タグ名 : ");
    scanf("%s", tag);

    printf("ファイルパス : ");
    scanf("%s", path);

    if (mode == 1) {
        addNewTag(tag, path);
    } else {
        addFileToTag(tag, path);
    }

    saveTags();

    printf("保存した\n");

    return;
}

void time_management(){

    FILE *fp = fopen("tag.txt", "r");

    if (fp == NULL) {
        printf("tag.txtが開けません\n");
        return;
    }

    char line[512];
    char tags[50][50];
    int tagCount = 0;

    printf("=== タグ一覧 ===\n");

    // タグ一覧取得
    while (fgets(line, sizeof(line), fp)) {

        char tag[50];
        sscanf(line, "%s", tag);

        strcpy(tags[tagCount], tag);

        printf("%d : %s\n", tagCount, tag);

        tagCount++;
    }

    rewind(fp);

    int select;
    printf("\nタグ番号 : ");
    scanf("%d", &select);

    if (select < 0 || select >= tagCount) {
        printf("無効な番号\n");
        return;
    }

    char targetTag[50];
    strcpy(targetTag, tags[select]);

    printf("\n=== %s のファイル ===\n", targetTag);

    while (fgets(line, sizeof(line), fp)) {

        char tag[50];

        char *p = strtok(line, " \n");

        if (p == NULL) continue;

        strcpy(tag, p);

        if (strcmp(tag, targetTag) == 0) {

            int i = 0;

            while ((p = strtok(NULL, " \n")) != NULL) {
                printf("%d : %s\n", i, p);
                i++;
            }

            break;
        }
    }

    fclose(fp);

    return;
}