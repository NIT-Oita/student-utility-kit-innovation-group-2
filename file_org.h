#ifndef FILE_ORG
#define FILE_ORG

int rmdir(const char* path);
int mkdir(const char* path);
int move(const char *old_path, const char *new_path);
int org(void);
void trim_newline(char *str);

#endif
