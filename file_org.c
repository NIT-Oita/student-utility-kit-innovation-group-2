#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <direct.h>
#include <errno.h>
#include <sys/stat.h>
 
#include "log.h"
#include "file_org.h"
 
 
#define MAX_PATH_LEN 512
 
 
/* 改行削除 */
void trim_newline(char *str)
{
    size_t len = strlen(str);
 
    if(len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
    }
}
 
 
/*
    設定読み込み
 
    target.txt
 
    file C:\test
    safe_copy C:\backup
 
*/
int load_config(Config *cfg)
{
    FILE *fp;
    char line[1024];
 
 
    fp = fopen("target.txt","r");
 
    if(fp == NULL)
    {
        return 1;
    }
 
 
    cfg->target_dir[0] = '\0';
    cfg->backup_dir[0] = '\0';
 
 
 
    while(fgets(line,sizeof(line),fp))
    {
        trim_newline(line);
 
 
        if(strncmp(line,"file ",5)==0)
        {
            strncpy(
                cfg->target_dir,
                line + 5,
                sizeof(cfg->target_dir)-1
            );
        }
 
 
        if(strncmp(line,"safe_copy ",10)==0)
        {
            strncpy(
                cfg->backup_dir,
                line + 10,
                sizeof(cfg->backup_dir)-1
            );
        }
    }
 
 
    fclose(fp);
 
 
 
    if(strlen(cfg->target_dir)==0 ||
       strlen(cfg->backup_dir)==0)
    {
        return 1;
    }
 
 
    return 0;
}
 
 
 
/*
    フォルダ作成
*/
int mk_dir(const char *path)
{
 
    if(_mkdir(path)==0)
    {
        log_write("mkdir",path,"");
        return 0;
    }
 
 
    if(errno==EEXIST)
    {
        return 0;
    }
 
 
    return 1;
}
 
 
 
/*
    ファイル移動
*/
int move_file(
    const char *old_path,
    const char *new_path
)
{
 
    if(rename(old_path,new_path)==0)
    {
        log_write(
            "move",
            old_path,
            new_path
        );
 
        return 0;
    }
 
 
    return 1;
}
 
 
 
/*
    バックアップ削除
*/
int delete_backup(const char *path)
{
 
    char cmd[MAX_PATH_LEN + 50];
 
 
    struct stat st;
 
 
    if(path==NULL)
    {
        return 1;
    }
 
 
    if(stat(path,&st)!=0)
    {
        return 0;
    }
 
 
 
    snprintf(
        cmd,
        sizeof(cmd),
        "rmdir /S /Q \"%s\" > nul 2>&1",
        path
    );
 
 
    if(system(cmd)!=0)
    {
        return 1;
    }
 
 
    return 0;
}
 
 
 
/*
    バックアップ作成
*/
int backup_folder(
    const char *src,
    const char *dst
)
{
 
    char cmd[2048];
 
 
    if(src==NULL || dst==NULL)
    {
        return 1;
    }
 
 
 
    snprintf(
        cmd,
        sizeof(cmd),
        "xcopy \"%s\" \"%s\" /E /I /Y > nul",
        src,
        dst
    );
 
 
    if(system(cmd)!=0)
    {
        return 1;
    }
 
 
    return 0;
}
 
 
 
/*
    復元
*/
int restore_folder(
    const char *backup,
    const char *target
)
{
 
    char cmd[2048];
 
 
    if(backup==NULL ||
       target==NULL)
    {
        return 1;
    }
 
 
 
    snprintf(
        cmd,
        sizeof(cmd),
        "xcopy \"%s\" \"%s\" /E /I /Y > nul",
        backup,
        target
    );
 
 
 
    if(system(cmd)!=0)
    {
        return 1;
    }
 
 
    return 0;
}
 
 
 
/*
    ルートチェック
*/
int is_root_path(const char *path)
{
 
    if(path==NULL)
    {
        return 1;
    }
 
 
    if(strlen(path)==3 &&
       path[1]==':' &&
       (path[2]=='\\' ||
        path[2]=='/'))
    {
        return 1;
    }
 
 
    return 0;
}
/*
    整理処理
 
    例:
 
    test.jpg
 
    ↓
 
    jpg/test.jpg
 
*/
int organize_files(const char *target_dir)
{
 
    DIR *dir;
    struct dirent *entry;
 
 
    if(target_dir==NULL)
    {
        return 1;
    }
 
 
 
    dir = opendir(target_dir);
 
 
    if(dir==NULL)
    {
        return 1;
    }
 
 
 
    while((entry=readdir(dir)) != NULL)
    {
 
        char *filename = entry->d_name;
 
 
 
        if(strcmp(filename,".")==0 ||
           strcmp(filename,"..")==0)
        {
            continue;
        }
 
 
 
        if(is_excluded_file(filename))
        {
            continue;
        }
 
 
 
        char old_path[MAX_PATH_LEN];
 
        snprintf(
            old_path,
            sizeof(old_path),
            "%s\\%s",
            target_dir,
            filename
        );
 
 
 
        struct stat st;
 
 
 
        if(stat(old_path,&st)!=0)
        {
            continue;
        }
 
 
 
        /*
            フォルダは無視
 
            作成したjpgフォルダを
            再処理しない
        */
        if(S_ISDIR(st.st_mode))
        {
            continue;
        }
 
 
 
 
        char *ext = strrchr(filename,'.');
 
 
        if(ext==NULL)
        {
            continue;
        }
 
 
 
        char ext_name[64];
 
        strcpy(
            ext_name,
            ext+1
        );
 
 
 
        char folder[MAX_PATH_LEN];
 
 
        snprintf(
            folder,
            sizeof(folder),
            "%s\\%s",
            target_dir,
            ext_name
        );
 
 
 
        if(mk_dir(folder))
        {
            closedir(dir);
            return 1;
        }
 
 
 
        char new_path[MAX_PATH_LEN];
 
 
        snprintf(
            new_path,
            sizeof(new_path),
            "%s\\%s\\%s",
            target_dir,
            ext_name,
            filename
        );
 
 
 
        if(move_file(old_path,new_path))
        {
            closedir(dir);
            return 1;
        }
 
    }
 
 
    closedir(dir);
 
 
    return 0;
}
 
 
 
 
 
/*
    メイン処理
*/
int org(void)
{
 
    Config cfg;
 
 
 
    if(load_config(&cfg))
    {
        printf(
            "設定ファイルエラー\n"
        );
 
        return 1;
    }
 
 
 
 
    if(strcmp(
        cfg.target_dir,
        cfg.backup_dir
    )==0)
    {
        printf(
            "同じフォルダです\n"
        );
 
        return 1;
    }
 
 
 
 
    if(is_root_path(cfg.backup_dir))
    {
        printf(
            "ドライブ直下は禁止です\n"
        );
 
        return 1;
    }
 
 
 
 
    printf(
        "対象 : %s\n",
        cfg.target_dir
    );
 
 
    printf(
        "バックアップ : %s\n",
        cfg.backup_dir
    );
 
 
 
    printf(
        "実行しますか？ y/n : "
    );
 
 
 
    char ans[16];
 
 
 
    fgets(
        ans,
        sizeof(ans),
        stdin
    );
 
 
 
    if(ans[0]!='y' &&
       ans[0]!='Y')
    {
        printf(
            "中止\n"
        );
 
        return 1;
    }
 
 
 
 
    printf(
        "バックアップ作成\n"
    );
 
 
 
    delete_backup(
        cfg.backup_dir
    );
 
 
 
    if(backup_folder(
        cfg.target_dir,
        cfg.backup_dir
    ))
    {
        printf(
            "バックアップ失敗\n"
        );
 
        return 1;
    }
 
 
 
 
    printf(
        "整理開始\n"
    );
 
 
 
    if(organize_files(cfg.target_dir))
    {
 
        printf(
            "エラー\n"
        );
 
 
        printf(
            "復元します\n"
        );
 
 
        delete_backup(
            cfg.target_dir
        );
 
 
        restore_folder(
            cfg.backup_dir,
            cfg.target_dir
        );
 
 
        return 1;
    }
 
 
 
    printf(
        "整理完了\n"
    );
 
 
    return 0;
}
 
 
 
 
 
/*
    バックアップから復元
*/
int backup_start(void)
{
 
    Config cfg;
 
 
    if(load_config(&cfg))
    {
        return 1;
    }
 
 
 
    struct stat st;
 
 
 
    if(stat(
        cfg.backup_dir,
        &st
    )!=0)
    {
        printf(
            "バックアップなし\n"
        );
 
        return 1;
    }
 
 
 
    delete_backup(
        cfg.target_dir
    );
 
 
 
    if(restore_folder(
        cfg.backup_dir,
        cfg.target_dir
    ))
    {
        return 1;
    }
 
 
 
    printf(
        "復元完了\n"
    );
 
 
    return 0;
}
 
 
 
 
 
 
/*
    除外ファイル追加
*/
int add_exclude_file(
    const char *filename
)
{
 
    FILE *fp;
 
 
 
    fp=fopen(
        "exclude.txt",
        "a"
    );
 
 
    if(fp==NULL)
    {
        return 1;
    }
 
 
 
    fprintf(
        fp,
        "%s\n",
        filename
    );
 
 
 
    fclose(fp);
 
 
    return 0;
}
 
 
 
 
 
 
/*
    除外チェック
*/
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
 
 
 
    if(fp==NULL)
    {
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
 
 
 
        if(strcmp(
            line,
            filename
        )==0)
        {
 
            fclose(fp);
 
            return 1;
        }
    }
 
 
 
    fclose(fp);
 
 
    return 0;
}
 
 
 
 
/*
    対象フォルダ設定
*/
int set_target_folder(
    const char *path
)
{
 
    FILE *fp;
 
 
 
    fp=fopen(
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
 
 
 
    fprintf(
        fp,
        "safe_copy backup\n"
    );
 
 
 
    fclose(fp);
 
 
 
    return 0;
}
 