#include <stdio.h>
#include<string.h>
#include <dirent.h>
#include <direct.h>
#include "log.h"
#include "file_org.h"

//ファイル削除
int rm_dir(const char* path){
    if(remove(path) == 0){
        return 0;
    } else {
        return 1;
    }
}

//ファイル作成
int mk_dir(const char* dir_path) {
    // mkdir関数を実行（正常終了なら0が返る）
    if (_mkdir(dir_path) == 0) {
        log_write("mkdir",dir_path,"");
        return 0;
    } else {
        return 1;
    }
}

//ファイル移動
int move(const char *old_path, const char *new_path) {
    // rename関数は成功すると0、失敗すると0以外を返す
    int rnm = rename(old_path, new_path);
    if(rnm == 0){
        log_write("move",old_path,new_path);
    }
    return rnm;
}

// 改行文字を削除する関数
void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

int org(void) {
    char target_dir[512];

    //target.txtを開く
    FILE *target_file = fopen("target.txt", "r");
    if (target_file == NULL) {
        perror("target.txt を開けませんでした");
        return 1;
    }

    // テキストファイルからフォルダのパスを読み込む
    if (fgets(target_dir, sizeof(target_dir), target_file) == NULL) {
        printf("設定ファイルが空です。\n");
        fclose(target_file);
        return 1;
    }
    fclose(target_file);

    trim_newline(target_dir); // 読み込んだパス末尾の改行を取り除く

    printf("対象フォルダ: %s\n\n", target_dir);

    // 読み込んだフォルダを開く
    DIR *dir = opendir(target_dir);
    if (dir == NULL) {
        perror("対象フォルダを開けませんでした");
        return 1;
    }

    struct dirent *entry;

    // フォルダ内のファイルを1つずつループ
    while ((entry = readdir(dir)) != NULL) {
        char *filename = entry->d_name;

        // "." と ".." は無視
        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
            continue;
        }

        // 拡張子を見つける
        char *ext = strrchr(filename, '.');
        if (ext == NULL) continue; // 拡張子がない場合はスキップ

        // ドット無しの拡張子名を取得
        char ext_name[64];
        strcpy(ext_name, ext + 1); 

        char new_dir[512];
        sprintf(new_dir, "%s/%s", target_dir, ext_name);
        
        // フォルダ作成
        mk_dir(new_dir);

        // 移動前パス
        char old_path[512];
        sprintf(old_path, "%s/%s", target_dir, filename);
        
        // 移動後パス
        char new_path[512];
        sprintf(new_path, "%s/%s/%s", target_dir, ext_name, filename);
        
        // ファイル移動
        if(move(old_path, new_path)){
            printf("エラー\n");
        }
    }

    closedir(dir);
    printf("\n分類完了\n");
    return 0;
}