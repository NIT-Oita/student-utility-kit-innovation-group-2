# 学生生活支援キット開発プロジェクト (Student Utility Kit Innovation)

> ⚠️ **NOTICE / 注意**
> このREADMEはテンプレートです。**自分たちのプロジェクトの内容に書き換えてください**。
> （タイトル、テーマ、メンバー名、ファイル名、ビルド手順、デモ動リンクなどを差し替える）
> *This is a template. Please edit and customize every section for your actual project.*

---

## 1. プロジェクト概要

対象フォルダ直下のファイルを拡張子ごとに自動分類し、指定したフォルダへ整理するコマンドラインツールです。
レポート、画像、ソースコードなどのファイル管理を効率化することを目的としています。

- 期間: 3週間
- 班構成: 3名
- 評価: 100点満点

---

## 2. テーマ一覧（班ごとに1つ・重複なし）

| # | テーマ | 内容 |
|---|---|---|
| 06 | ファイル整理キット | 拡張子・日付で自動分類 |
---

## 3. 班構成（役割分担）

| メンバー | 役割 | 担当ファイル |
|---|---|---|
| 江島 | ログ管理機能の実装、UIの作成 | 'log.c' 'log.h' 'ui.c' 'ui.h' |
| 佐藤 | ファイル整理機能の実装、Makefileの作成 | 'file_org.c' 'file_org.h' 'Makefile' |
| 橋本 | タグとファイルの登録・追加の機能、入力チェック | 'tag.c'  'tag.h'' |

全員がGitで互いのプルリクエストをレビュー。

---

## 4. ビルド方法

### 必要なもの

- gcc
- `make`
- Git

### プラットフォーム別セットアップ

**Windows (MSYS2 / MinGW)**
```bash
pacman -S mingw-w64-x86_64-gcc make
```

**macOS**
```bash
xcode-select --install
```

**Linux (Ubuntu)**
```bash
sudo apt install build-essential
```

### ビルドと実行

```bash
git clone https://github.com/NIT-Oita/student-utility-kit-innovation-group-2.git
cd student-utility-kit-innvation-group-2
make          # コンパイル
taskman.exe     # 実行（Windowsは taskman.exe）
```

### Makefile ターゲット

| コマンド | 動作 |
|---|---|
| `make` | コンパイル＆リンク |
| `make done` | ビルドして実行 |
| `make clean` | 中間ファイルを削除 |

---

## 5. プロジェクト構成

```
your-project/
├── Makefile
├── README.md
├── file_org.c  file_org.h
├── ui.c  ui.h
├── log.c  log.h
├── tag.c  tag.h
└── main.c
```

---

## 6. データ形式（例）

`data/tasks.csv` の形式（自分のプロジェクトに合わせて書き換える）:

このプロジェクトではCSVなどのデータファイルは使用しない。

---

## 7. Gitワークフロー

```bash
git pull origin main                    # 最新を取得
git checkout -b feat/<feature-name>     # 機能ごとにブランチ
# ... コード変更 ...
git commit -m "add: <意味のあるメッセージ>"
git push -u origin feat/<feature-name>  # PR作成 → レビュー → マージ
```

main ブランチには直接 push しない。

---

## 8. テストチェックリスト

- [ ] 存在しないフォルダを指定しても落ちない
- [ ] 空フォルダでも正常終了する
- [ ] 同名ファイルがある場合に適切に処理する
- [ ] 対応していない拡張子を無視できる

推奨コンパイルオプション:
```bash
gcc -Wall -Wextra -fsanitize=address ...
```

---

## 9. 評価基準（100点）

| 項目 | 配点 | 内容 |
|---|---:|---|
| 機能性 | 30 | 全機能が正しく動作 |
| コード品質 | 20 | モジュール化・警告ゼロ・命名 |
| Git共同作業 | 15 | ブランチ・PR・3人均等のコミット |
| テスト・堅牢性 | 15 | 不正入力で落ちない |
| ドキュメント | 10 | README・Makefile・コメント |
| デモ・独創性 | 10 | 5分ライブデモ |
| **合計** | **100** | 合格: 60点／優秀: 85点以上 |

---


---

## 10. メンバー / Members

> **編集してください** — Edit this section

- **江島　綾音 (s2508)** — `s2508@oita.kosen-ac.jp`
- **佐藤　成将 (s2522)** — `s2522@oita.kosen-ac.jp`
- **橋本　昌樹 (s2532)** — `s2532@oita.kosen-ac.jp`

班番号: **Group 02**

---

## 12. デモ動画 / Demo

> プロジェクト完成後、ここにデモ動画またはスクリーンショットを追加してください。
<img width="1493" height="879" alt="image" src="https://github.com/user-attachments/assets/5cc12868-fd84-4035-b9c3-5e79ccd7dc62" />
<img width="849" height="354" alt="image" src="https://github.com/user-attachments/assets/840a4f56-ed32-404d-9b24-56da71e7a287" />
<img width="574" height="348" alt="image" src="https://github.com/user-attachments/assets/bcd2f6cd-1709-4623-9729-d8c9166c83f4" />
<img width="875" height="630" alt="image" src="https://github.com/user-attachments/assets/e835e061-05d1-41ed-ae11-c1de8553617e" />
<img width="840" height="429" alt="image" src="https://github.com/user-attachments/assets/c9509e69-b694-498c-b3ce-807ebf891c3a" />


---

## ライセンス

学内提出用。商用利用なし。
