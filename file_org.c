#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <direct.h>
#include <errno.h>
#include <sys/stat.h>

#include "log.h"
#include "file_org.h"


void trim_newline(char *str)
{
size_t len = strlen(str);


if(len > 0 && str[len - 1] == '\n')
{
    str[len - 1] = '\0';
}


}


int load_config(Config *cfg)
{
FILE *fp = fopen("target.txt","r");


if(fp == NULL)
{
    return 1;
}

cfg->target_dir[0] = '\0';
cfg->backup_dir[0] = '\0';

char line[1024];

while(fgets(line,sizeof(line),fp))
{
    trim_newline(line);

    /* file 行を発見 */
    if(strncmp(line,"file ",5) == 0)
    {
        strcpy(cfg->target_dir,line + 5);
    }

    /* safe_copy 行を発見 */
    else if(strncmp(line,"safe_copy ",10) == 0)
    {
        strcpy(cfg->backup_dir,line + 10);
    }
}

fclose(fp);

/* 必須項目が存在しない場合は失敗 */
if(strlen(cfg->target_dir) == 0 ||
   strlen(cfg->backup_dir) == 0)
{
    return 1;
}

return 0;


}

int rm_dir(const char *path)
{
    if(rmdir(path) == 0)
    {
        log_write("rmdir", path, "");
        return 0;
    }

    return 1;
}

int mk_dir(const char *path)
{
    if(_mkdir(path) == 0)
    {
        log_write("mkdir", path, "");
        return 0;
    }

    if(errno == EEXIST)
    {
        return 0;
    }

    return 1;
}

int move(const char *old_path, const char *new_path)
{
    if(rename(old_path, new_path) == 0)
    {
        log_write("move", old_path, new_path);
        return 0;
    }

    return 1;
}

/*==================================================
delete_backup()


古いバックアップを削除する。

毎回新しいバックアップを作成するため
前回のバックアップを消しておく。


==================================================*/
int delete_backup(const char *backup_dir)
{
char cmd[1024];


sprintf(
    cmd,
    "rmdir /S /Q \"%s\" > nul 2>&1",
    backup_dir
);

return system(cmd);


}

/*==================================================
backup_folder()


整理前の状態を保存する。

xcopy を利用してフォルダごとコピーする。

戻り値
    0 : 成功
    0以外 : 失敗


==================================================*/
int backup_folder(
const char *src,
const char *dst)
{
char cmd[2048];


sprintf(
    cmd,
    "xcopy \"%s\" \"%s\" /E /I /Y > nul",
    src,
    dst
);

return system(cmd);


}

/*==================================================
restore_folder()


バックアップから復元する。

整理中にエラーが発生した場合に呼ばれる。


==================================================*/
int restore_folder(
const char *backup,
const char *target)
{
char cmd[2048];


sprintf(
    cmd,
    "xcopy \"%s\" \"%s\" /E /I /Y > nul",
    backup,
    target
);

return system(cmd);


}

/*==================================================
organize_files()


対象フォルダ内のファイルを調べる。

拡張子ごとにフォルダを作成し、
対応するフォルダへ移動する。

例

    image.jpg
        ↓

    jpg/image.jpg

戻り値
    0 : 成功
    1 : エラー


==================================================*/
int organize_files(const char *target_dir)
{
DIR *dir = opendir(target_dir);


if(dir == NULL)
{
    return 1;
}

struct dirent *entry;

while((entry = readdir(dir)) != NULL)
{
    char *filename = entry->d_name;

    /* 特殊フォルダは無視 */
    if(strcmp(filename,".") == 0 ||
       strcmp(filename,"..") == 0)
    {
        continue;
    }

    /* 拡張子検索 */
    char full_path[MAX_PATH_LEN];

    sprintf(
        full_path,
        "%s/%s",
        target_dir,
        filename
    );

    struct stat st;

    if(stat(full_path, &st) == 0)
    {
        if(st.st_mode & S_IFDIR)
        {
            continue;
        }
    }

    char *ext = strrchr(filename,'.');

    if(ext == NULL)
    {
        continue;
    }

    /* ドットを除いた拡張子 */
    char ext_name[64];
    strcpy(ext_name,ext + 1);

    /* 作成するフォルダ */
    char new_dir[MAX_PATH_LEN];

    sprintf(
        new_dir,
        "%s/%s",
        target_dir,
        ext_name
    );

    if(mk_dir(new_dir))
    {
        closedir(dir);
        return 1;
    }

    /* 移動元 */
    char old_path[MAX_PATH_LEN];

    sprintf(
        old_path,
        "%s/%s",
        target_dir,
        filename
    );

    /* 移動先 */
    char new_path[MAX_PATH_LEN];

    sprintf(
        new_path,
        "%s/%s/%s",
        target_dir,
        ext_name,
        filename
    );

    if(move(old_path,new_path))
    {
        closedir(dir);
        return 1;
    }
}

closedir(dir);

return 0;


}

/*==================================================
org()


システム全体を制御するメイン関数

処理手順

① 設定読込
② バックアップ作成
③ ファイル整理
④ エラーなら復元


==================================================*/
int org(void)
{
    Config cfg;

    /* 設定読込 */
    if(load_config(&cfg))
    {
        printf("設定ファイルエラー\n");
        return 1;
    }

    printf("対象フォルダ : %s\n", cfg.target_dir);
    printf("バックアップ : %s\n", cfg.backup_dir);

    /* 古いバックアップ削除 */
    delete_backup(cfg.backup_dir);

    /* バックアップ作成 */
    if(backup_folder(
            cfg.target_dir,
            cfg.backup_dir))
    {
        printf("バックアップ失敗\n");
        return 1;
    }

    /* ファイル整理 */
    if(organize_files(cfg.target_dir))
    {
        printf("エラー発生\n");
        printf("復元開始\n");

        /* 整理途中のフォルダを削除 */
        delete_backup(cfg.target_dir);

        /* バックアップから復元 */
        restore_folder(
            cfg.backup_dir,
            cfg.target_dir
        );

        return 1;
    }

    printf("整理完了\n");

    return 0;
}