#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "ui.h"
#include "log.h"
#include "tag.h"
#include "file_org.h"

void wait_back_menu();
void home_menu();
void classify_menu();
void tag_menu();
void name(void);

// テスト用
void name(void)
{
    printf("DEMO\n");
    printf("まだ実装されていません\n");

    log_write(
        "CLASSIFY_DEMO",
        "sample.txt",
        "txt/sample.txt"
    );

    printf("\nデモログを記録しました\n");

    wait_back_menu();
}

int main() {

    // 保存されたタグデータを読み込む
    loadTags();

    // 起動画面
    printf("============================\n");
    printf("      ファイル整理ツール      \n");
    printf("============================\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    printf("    Press Enter to Start    \n");

    // Enter キーが読まれるまで，待つ
    while (1) {
        int key = _getch();
        if (key == 13) {   // Enter キー
            break;
        }
    }

    system("cls"); // 画面クリア
    home_menu(); // ホーム画面へ進む
    return 0; // 終了
}

// メニュー画面に戻る前の確認
void wait_back_menu(){
    printf("メニュー選択画面に戻りますか（0）\n");
    while (1) {
        int key = _getch();
            if (key == '0') {
                break; // 0 が押されたら抜ける
            }
    }
}

// ホームメニュー
void home_menu() {

    const char *menu[] = {
        "拡張子で分類する",
        "タグ管理",
        "ファイル移動履歴を確認する",
        "終了"
    };

    // 項目数を取得
    int count = sizeof(menu) / sizeof(menu[0]);

    // 選択されたメニューを実行する
    while (1) {
        int choice = select_menu(menu, count);
        system("cls"); // 画面クリア
        switch (choice) {
            case 0:
                classify_menu();
                break;

            case 1:
                tag_menu();
                break;

            case 2:
                log_print();
                wait_back_menu();
                break;

            case 3:
                return; // 終了
        }
    }
}

// 分類メニュー関数
void classify_menu() {

    const char *menu[] = {
        "分類を開始する",
        "管理フォルダの設定",
        "管理対象外ファイルの設定",
        "バックアップの実行",
        "戻る"
    };

    // 項目数を取得
    int count = sizeof(menu) / sizeof(menu[0]);

    // 選択されたメニューを実行する
    while (1) {
        int choice = select_menu(menu, count);

        system("cls");
        switch (choice) {
            case 0:
                org();
                break;

            case 1:
                char path[512];

                printf("管理フォルダを入力: ");
                scanf("%511s", path);

                if(set_target_folder(path))
                {
                    printf("設定に失敗しました\n");
                }
                else
                {
                    printf("設定しました\n");
                }
                break;

            case 2:
                /*char filename[256];

                printf("除外ファイル名を入力: ");
                scanf("%255s", filename);

                add_excluded_file(filename);
                if(add_exclude_file(filename))
                {
                    printf("追加に失敗しました\n");
                }
                else
                {
                    printf("追加しました\n");
                }*/
                break;

            case 3:
                // backup_start(); 
                break;

            case 4:
                return; // ホーム画面に戻る
        }
    }
}

// タグメニュー関数
void tag_menu() {

    char tag[50];
    char file[256];

    const char *menu[] = {
        "タグ一覧",
        "タグを作成する",
        "タグを削除する",
        "タグにファイルを追加する",
        "タグからファイルを削除する",
        "戻る"
    };

    // 項目数を取得
    int count = sizeof(menu) / sizeof(menu[0]);

     // 選択されたメニューを実行する
    while (1) {
        int choice = select_menu(menu, count);

        system("cls");
        switch (choice) {
            case 0:
                // タグ一覧を表示する
                listTags();
                wait_back_menu();
                break;

            case 1:
                // タグを作成する
                printf("タグ名を入力: ");
                scanf("%49s", tag);

                selectFile(file);

                createTag(tag, file);
                saveTags();

                printf("タグを作成しました\n");
                wait_back_menu();
                break;

            case 2:
                // タグを削除する
                selectTag(tag);

                deleteTag(tag);
                saveTags();

                printf("タグを削除しました\n");
                wait_back_menu();
                break;

            case 3:
                // タグにファイルを追加する
                selectTag(tag);
                selectFile(file);

                addFile(tag, file);
                saveTags();

                printf("ファイルを追加しました\n");
                wait_back_menu();
                break;

            case 4:
                // タグからファイルを削除する
                selectTag(tag);

                removeFile(tag, file);
                saveTags();

                printf("ファイルを削除しました\n");
                wait_back_menu();
                break;

            case 5:
                return; // ホーム画面に戻る
        }
    }
}