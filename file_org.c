#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <direct.h>
#include <errno.h>

#define MAX_PATH 512

void log_write(const char *action, const char *src, const char *dst);

int mk_dir(const char *path)
{
    if (_mkdir(path) == 0)
    {
        log_write("mkdir", path, "");
        return 0;
    }

    if (errno == EEXIST)
        return 0;

    return 1;
}

int move_file(const char *src, const char *dst)
{
    if (rename(src, dst) == 0)
    {
        log_write("move", src, dst);
        return 0;
    }

    log_write("error_move", src, dst);
    return 1;
}

int org(const char *target_dir)
{
    DIR *dir = opendir(target_dir);
    if (!dir)
    {
        log_write("error_opendir", target_dir, "");
        return 1;
    }

    log_write("start", target_dir, "");

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        char *name = entry->d_name;

        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
            continue;

        char full_path[MAX_PATH];
        snprintf(full_path, sizeof(full_path), "%s/%s", target_dir, name);

        struct stat st;

        if (stat(full_path, &st) != 0)
        {
            log_write("error_stat", full_path, "");
            continue;
        }

        // フォルダは無視
        if (S_ISDIR(st.st_mode))
        {
            continue;
        }

        char *ext = strrchr(name, '.');

        if (!ext)
        {
            log_write("skip_no_ext", name, "");
            continue;
        }

        ext++; // 拡張子だけ

        // フォルダ作成
        char folder[MAX_PATH];
        snprintf(folder, sizeof(folder), "%s/%s", target_dir, ext);

        mk_dir(folder);

        // 移動先作成
        char new_path[MAX_PATH];
        snprintf(new_path, sizeof(new_path), "%s/%s/%s", target_dir, ext, name);

        move_file(full_path, new_path);
    }

    closedir(dir);

    log_write("end", target_dir, "");

    return 0;
}