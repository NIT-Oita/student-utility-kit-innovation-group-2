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

    if(strncmp(line,"file ",5) == 0)
    {
        strncpy(
            cfg->target_dir,
            line + 5,
            sizeof(cfg->target_dir) - 1
        );
    
        cfg->target_dir[
            sizeof(cfg->target_dir) - 1
        ] = '\0';
    }
    
    else if(strncmp(line,"safe_copy ",10) == 0)
    {
        strncpy(
            cfg->backup_dir,
            line + 10,
            sizeof(cfg->backup_dir) - 1
        );
    
        cfg->backup_dir[
            sizeof(cfg->backup_dir) - 1
        ] = '\0';
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
    struct stat st;
 
    if(backup_dir == NULL)
    {
        return 1;
    }
 
    if(backup_dir[0] == '\0')
    {
        return 1;
    }
 
    /* フォルダが存在しないなら成功扱い */
    if(stat(backup_dir, &st) != 0)
    {
        return 0;
    }
 
    int ret = snprintf(
        cmd,
        sizeof(cmd),
        "rmdir /S /Q \"%s\" > nul 2>&1",
        backup_dir
    );
 
    if(ret < 0 ||
       ret >= (int)sizeof(cmd))
    {
        return 1;
    }
 
    ret = system(cmd);
 
    return (ret == 0) ? 0 : 1;
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
    int ret;
 
    if(src == NULL || dst == NULL)
    {
        return 1;
    }
 
    ret = snprintf(
        cmd,
        sizeof(cmd),
        "xcopy \"%s\" \"%s\" /E /I /Y > nul",
        src,
        dst
    );
 
    if(ret < 0 ||
       ret >= (int)sizeof(cmd))
    {
        return 1;
    }
 
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
    int ret;
 
    if(backup == NULL || target == NULL)
    {
        return 1;
    }
 
    ret = snprintf(
        cmd,
        sizeof(cmd),
        "xcopy \"%s\" \"%s\" /E /I /Y > nul",
        backup,
        target
    );
 
    if(ret < 0 ||
       ret >= (int)sizeof(cmd))
    {
        return 1;
    }
 
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
    if(target_dir == NULL)
    {
        return 1;
    }
    
    if(target_dir[0] == '\0')
    {
        return 1;
    }

    struct stat st;

    if(stat(target_dir, &st) != 0){
        return 1;
    }

    DIR *dir = opendir(target_dir);

    if(dir == NULL)
    {
        return 1;
    }

    struct dirent *entry;

    while((entry = readdir(dir)) != NULL)
    {
        char *filename = entry->d_name;

        if(
            is_excluded_file(
                filename
            )
        )
        {
            continue;
        }

        /* 特殊フォルダは無視 */
        if(strcmp(filename,".") == 0 ||
        strcmp(filename,"..") == 0)
        {
            continue;
        }

        /* 拡張子検索 */
        char full_path[MAX_PATH_LEN];

        snprintf(
            full_path,
            sizeof(full_path),
            "%s/%s",
            target_dir,
            filename
        );

        struct stat st;

        if(S_ISDIR(st.st_mode))
        {
            if(organize_files(full_path))
            {
                closedir(dir);
                return 1;
            }
        
            continue;
        }
 

        char *ext = strrchr(filename,'.');

        if(ext == NULL)
        {
            continue;
        }

        /* ドットを除いた拡張子 */
        char ext_name[64];
        
        strncpy(
            ext_name,
            ext + 1,
            sizeof(ext_name) - 1
        );
        
        ext_name[
            sizeof(ext_name) - 1
        ] = '\0';

        /* 作成するフォルダ */
        char new_dir[MAX_PATH_LEN];

        snprintf(
            new_dir,
            sizeof(new_dir),
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

        snprintf(
            old_path,
            sizeof(old_path),
            "%s/%s",
            target_dir,
            filename
        );

        /* 移動先 */
        char new_path[MAX_PATH_LEN];

        snprintf(
            new_path,
            sizeof(new_path),
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

    /* 同じフォルダは禁止 */
    if(strcmp(
        cfg.target_dir,
        cfg.backup_dir
    ) == 0)
    {
        printf(
            "バックアップ先が対象フォルダと同じです\n"
        );
        return 1;
    }

    if(is_root_path(cfg.backup_dir))
    {
        printf(
            "ドライブ直下は指定できません\n"
        );
        return 1;
    }

    if(strncmp(
        cfg.backup_dir,
        cfg.target_dir,
        strlen(cfg.target_dir)
    ) == 0)
    {
        printf(
            "バックアップ先が対象フォルダ内です\n"
        );
        return 1;
    }

    printf("対象フォルダ : %s\n", cfg.target_dir);
    printf("バックアップ : %s\n", cfg.backup_dir);

    printf(
    "この内容で実行しますか？(y/n): "
    );
    
    char ans[8];
    
    if(
        fgets(
            ans,
            sizeof(ans),
            stdin
        )==NULL
    )
    {
        printf("入力エラー\n");
        return 1;
    }
    
    if(ans[0] != 'y' &&
    ans[0] != 'Y')
    {
        printf("中止しました\n");
        return 1;
    }

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
        if(
            restore_folder(
                cfg.backup_dir,
                cfg.target_dir
            )
        )
        {
            printf("復元失敗\n");
            return 1;
        }

        return 1;
    }

    printf("整理完了\n");

    return 0;
}

/*==================================================
backup_start()

残っているバックアップから復元する。

戻り値
    0 : 成功
    1 : 失敗

==================================================*/
int backup_start(void)
{
    Config cfg;

    if(load_config(&cfg))
    {
        return 1;
    }

    struct stat st;

    /* バックアップが存在しない */
    if(stat(cfg.backup_dir, &st) != 0)
    {
        printf("バックアップは存在しません\n");
        return 1;
    }

    printf("バックアップを発見\n");
    printf("復元開始\n");

    /* 現在の整理済みフォルダ削除 */
    delete_backup(cfg.target_dir);

    /* バックアップから復元 */
    if(restore_folder(
        cfg.backup_dir,
        cfg.target_dir))
    {
        printf("復元失敗\n");
        return 1;
    }

    printf("復元完了\n");

    return 0;
}

int is_root_path(const char *path)
{
    if(path == NULL)
    {
        return 1;
    }
 
    if(strlen(path) == 2 &&
       path[1] == ':')
    {
        return 1;
    }
 
    if(strlen(path) == 3 &&
       path[1] == ':' &&
       (path[2] == '\\' ||
        path[2] == '/'))
    {
        return 1;
    }
 
    return 0;
}

int set_target_folder(
    const char *path
)
{
    FILE *fp;

    if(path==NULL){
        return 1;
    }

    fp = fopen(
        "target.txt",
        "w"
    );

    if(fp==NULL)
    {
        return 1;
    }

    fprintf(
        fp,
        "file %s\n",
        path
    );

    fclose(fp);

    return 0;
}

int add_exclude_file(
    const char *filename
)
{
    if(filename==NULL)
    {
        return 1;
    }
 
    FILE *fp;

    fp=fopen(
        "exclude.txt",
        "a"
    );

    if(fp = NULL)
    {
        return 1;
    }

    fprintf
    (
        fp,
        "%s\n",
        filename
    );

    fclose(fp);

    return 0;
}

int is_excluded_file(
    const char *filename
)
{
    FILE *fp;
    char line[256];

    fp=fopen(
        "exclude.txt",
        "r"
    );

    if(fp==NULL){
        return 0;
    }

    while(
        fgets(
            line,
            sizeof(line),
            fp
        )
    )
    {
        trim_newline(line);

        if(
            strcmp(
                line,
                filename
            )==NULL
        )
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);

    return 0;
}