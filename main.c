#include <stdlib.h>
#include <conio.h>
#include "ui.h"
#include "log.h"

void home_menu();
void classify_menu();
void tag_menu();

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

                printf("メニュー選択画面に戻りますか（0）\n");
                while (1) {
                    int key = _getch();
                    if (key == '0') {
                        break; // 0 が押されたら抜ける
                    }
                }

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
                name();
                break;

            case 1:
                name();
                break;

            case 2:
                name();
                break;

            case 3:
                name();
                break;

            case 4:
                name();
                break;

            case 5:
                return; // ホーム画面に戻る
        }
    }
}