#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "ui.h"
#include "log.h"
#include "tag.h"

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

    printf("============================\n");
    printf("      ファイル整理ツール      \n");
    printf("============================\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    printf("    Press Enter to Start    \n");

    while (1) {
        int key = _getch();
        if (key == 13) {   // Enter キー
            break;
        }
    }
    system("cls");
    home_menu(); // ホーム画面へ進む
    return 0;
}

void wait_back_menu(){
    printf("メニュー選択画面に戻りますか（0）\n");
    while (1) {
        int key = _getch();
            if (key == '0') {
                break; // 0 が押されたら抜ける
            }
    }
}

void home_menu() {

    const char *menu[] = {
        "拡張子で分類する",
        "タグ管理",
        "ファイル移動履歴を確認する",
        "終了"
    };

    // 項目数を取得
    int count = sizeof(menu) / sizeof(menu[0]);

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

void classify_menu() {

    const char *menu[] = {
        "分類を開始する",
        "管理フォルダの設定",
        "管理対象外ファイルの設定",
        "戻る"
    };

    // 項目数を取得
    int count = sizeof(menu) / sizeof(menu[0]);

    while (1) {
        int choice = select_menu(menu, count);

        system("cls");
        switch (choice) {
            case 0:
                name();
                break;

            case 1:
                name(); 
                break;

            case 2:
                name(); 
                break;

            case 3:
                return; // ホーム画面に戻る
        }
    }
}

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

    int count = sizeof(menu) / sizeof(menu[0]);

    while (1) {
        int choice = select_menu(menu, count);

        system("cls");
        switch (choice) {
            case 0:
                listTags();
                wait_back_menu();
                break;

            case 1:
                printf("タグ名を入力: ");
                scanf("%49s", tag);

                selectFile(file);

                createTag(tag, file);
                saveTags();

                printf("タグを作成しました\n");
                wait_back_menu();
                break;

            case 2:
                selectTag(tag);

                deleteTag(tag);
                saveTags();

                printf("タグを削除しました\n");
                wait_back_menu();
                break;

            case 3:
                selectTag(tag);
                selectFile(file);

                addFile(tag, file);
                saveTags();

                printf("ファイルを追加しました\n");
                wait_back_menu();
                break;

            case 4:
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