#include <stdio.h>
#include <stdlib.h>
#include <windows.h>   // 画面クリアと色変更に使用
#include <conio.h>     // _getch() でキー入力を取得

// 色を変更する関数
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 矢印キーで選択
int select_menu(const char *items[], int count) {
    int index = 0;  // 現在選択中の項目

    while (1) {
        system("cls");  // 画面クリア

        printf("===== メニュー =====\n\n");

        // メニュー項目を表示
        for (int i = 0; i < count; i++) {
            if (i == index) {
                // 選択中の項目は色を変えて表示（緑）
                setColor(10);
                printf("> %s\n", items[i]);
                setColor(7);  // 色を元に戻す（白）
            } else {
                printf("  %s\n", items[i]);
            }
        }

        int key = _getch();  // Enter を押さずに，キー入力を取得

        // 数字キー（0?9）で即決定
        if (key >= '0' && key <= '9') {
            int num = key - '0';  // 0?9 に変換
            if (num < count) {
                return num;       // 即決定
            }
        }


        // 矢印キーは 0 または 224 の後にコードが来る
        if (key == 0 || key == 224) {
            key = _getch();  // 実際のキーコードを取得

            if (key == 72) {        // ↑キー
                index--;
                if (index < 0) index = count - 1;
            } else if (key == 80) { // ↓キー
                index++;
                if (index >= count) index = 0;
            }
        }
        // Enter が押されたら選択確定
        else if (key == 13) {
            return index;
        }
    }
}