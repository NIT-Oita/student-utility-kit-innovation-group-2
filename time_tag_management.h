#ifndef TAG_H
#define TAG_H

void loadTags(void);
void saveTags(void);

void listTags(void);

void createTag(char *tag, char *file);
void addFile(char *tag, char *file);

void deleteTag(char *tag);
void removeFile(char *tag, char *file);

void selectTag(char *out);
void selectFile(char *out);

#endif