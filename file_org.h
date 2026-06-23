#ifndef FILE_ORG_H
#define FILE_ORG_H

#define MAX_PATH_LEN 512

typedef struct
{
    char target_dir[MAX_PATH_LEN];
    char backup_dir[MAX_PATH_LEN];

} Config;

void trim_newline(char *str);

int load_config(Config *cfg);

int rm_dir(const char *path);

int mk_dir(const char *path);

int move(
    const char *old_path,
    const char *new_path
);

int delete_backup(
    const char *backup_dir
);

int backup_folder(
    const char *src,
    const char *dst
);

int restore_folder(
    const char *backup,
    const char *target
);

int organize_files(
    const char *target_dir
);

int set_target_folder(
    const char *path
);

int add_excluded_file(
    const char *filename
);

int is_excluded_file(
    const char *filename
);

int is_root_path(
    const char *path
);

int org(void);

int backup_start(void);

int add_exclude_file(const char *filename);

#endif