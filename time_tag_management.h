#ifndef TAG_H
#define TAG_H

void loadTags();
void saveTags();

void listTags();
void createTag(char *tag, char *file);
void deleteTag(char *tag);
void addFile(char *tag, char *file);
void removeFile(char *tag, char *file);

void selectFile(char *out);
void selectTag(char *out);

#endif