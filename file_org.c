#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <direct.h>
#include <errno.h>
#include <sys/stat.h>

#include "log.h"
#include "file_org.h"


void trim_newline(char *str)
{
size_t len = strlen(str);


if(len > 0 && str[len - 1] == '\n')
{
    str[len - 1] = '\0';
}


}


int load_config(Config *cfg)
{
FILE *fp = fopen("target.txt","r");


if(fp == NULL)
{
    return 1;
}

cfg->target_dir[0] = '\0';
cfg->backup_dir[0] = '\0';

char line[1024];

while(fgets(line,sizeof(line),fp))
{
    trim_newline(line);

    /* file �s�𔭌� */
    if(strncmp(line,"file ",5) == 0)
    {
        strcpy(cfg->target_dir,line + 5);
    }

    /* safe_copy �s�𔭌� */
    else if(strncmp(line,"safe_copy ",10) == 0)
    {
        strcpy(cfg->backup_dir,line + 10);
    }
}

fclose(fp);

/* �K�{���ڂ����݂��Ȃ��ꍇ�͎��s */
if(strlen(cfg->target_dir) == 0 ||
   strlen(cfg->backup_dir) == 0)
{
    return 1;
}

return 0;


}

int rm_dir(const char *path)
{
    if(rmdir(path) == 0)
    {
        log_write("rmdir", path, "");
        return 0;
    }

    return 1;
}

int mk_dir(const char *path)
{
    if(_mkdir(path) == 0)
    {
        log_write("mkdir", path, "");
        return 0;
    }

    if(errno == EEXIST)
    {
        return 0;
    }

    return 1;
}

int move(const char *old_path, const char *new_path)
{
    if(rename(old_path, new_path) == 0)
    {
        log_write("move", old_path, new_path);
        return 0;
    }

    return 1;
}

/*==================================================
delete_backup()


�Â��o�b�N�A�b�v���폜����B

����V�����o�b�N�A�b�v���쐬���邽��
�O��̃o�b�N�A�b�v�������Ă����B


==================================================*/
int delete_backup(const char *backup_dir)
{
    char cmd[1024];

    if (backup_dir == NULL)
    {
        return 1;
    }

    if (backup_dir[0] == '\0')
    {
        return 1;
    }

    if (strlen(backup_dir) >= 900)
    {
        return 1;
    }

    int ret = snprintf(
        cmd,
        sizeof(cmd),
        "rmdir /S /Q \"%s\" > nul 2>&1",
        backup_dir
    );

    if (ret < 0 || ret >= (int)sizeof(cmd))
    {
        return 1;
    }

    ret = system(cmd);

    if (ret != 0)
    {
        return 1;
    }

    return 0;
}
/*==================================================
backup_folder()


�����O�̏�Ԃ�ۑ�����B

xcopy �𗘗p���ăt�H���_���ƃR�s�[����B

�߂�l
    0 : ����
    0�ȊO : ���s


==================================================*/
int backup_folder(
const char *src,
const char *dst)
{
char cmd[2048];


sprintf(
    cmd,
    "xcopy \"%s\" \"%s\" /E /I /Y > nul",
    src,
    dst
);

return system(cmd);


}

/*==================================================
restore_folder()


�o�b�N�A�b�v���畜������B

�������ɃG���[�����������ꍇ�ɌĂ΂��B


==================================================*/
int restore_folder(
const char *backup,
const char *target)
{
char cmd[2048];


sprintf(
    cmd,
    "xcopy \"%s\" \"%s\" /E /I /Y > nul",
    backup,
    target
);

return system(cmd);


}

/*==================================================
organize_files()


�Ώۃt�H���_���̃t�@�C���𒲂ׂ�B

�g���q���ƂɃt�H���_���쐬���A
�Ή�����t�H���_�ֈړ�����B

��

    image.jpg
        ��

    jpg/image.jpg

�߂�l
    0 : ����
    1 : �G���[


==================================================*/
int organize_files(const char *target_dir)
{
DIR *dir = opendir(target_dir);


if(dir == NULL)
{
    return 1;
}

struct dirent *entry;

while((entry = readdir(dir)) != NULL)
{
    char *filename = entry->d_name;

    /* ����t�H���_�͖��� */
    if(strcmp(filename,".") == 0 ||
       strcmp(filename,"..") == 0)
    {
        continue;
    }

    /* �g���q���� */
    char full_path[MAX_PATH_LEN];

    sprintf(
        full_path,
        "%s/%s",
        target_dir,
        filename
    );

    struct stat st;

    if(stat(full_path, &st) == 0)
    {
        if(st.st_mode & S_IFDIR)
        {
            continue;
        }
    }

    char *ext = strrchr(filename,'.');

    if(ext == NULL)
    {
        continue;
    }

    /* �h�b�g���������g���q */
    char ext_name[64];
    strcpy(ext_name,ext + 1);

    /* �쐬����t�H���_ */
    char new_dir[MAX_PATH_LEN];

    sprintf(
        new_dir,
        "%s/%s",
        target_dir,
        ext_name
    );

    if(mk_dir(new_dir))
    {
        closedir(dir);
        return 1;
    }

    /* �ړ��� */
    char old_path[MAX_PATH_LEN];

    sprintf(
        old_path,
        "%s/%s",
        target_dir,
        filename
    );

    /* �ړ��� */
    char new_path[MAX_PATH_LEN];

    sprintf(
        new_path,
        "%s/%s/%s",
        target_dir,
        ext_name,
        filename
    );

    if(move(old_path,new_path))
    {
        closedir(dir);
        return 1;
    }
}

closedir(dir);

return 0;


}

/*==================================================
org()


�V�X�e���S�̂𐧌䂷�郁�C���֐�

�����菇

�@ �ݒ�Ǎ�
�A �o�b�N�A�b�v�쐬
�B �t�@�C������
�C �G���[�Ȃ畜��


==================================================*/
int org(void)
{
    Config cfg;

    /* �ݒ�Ǎ� */
    if(load_config(&cfg))
    {
        printf("�ݒ�t�@�C���G���[\n");
        return 1;
    }

    printf("�Ώۃt�H���_ : %s\n", cfg.target_dir);
    printf("�o�b�N�A�b�v : %s\n", cfg.backup_dir);

    /* �Â��o�b�N�A�b�v�폜 */
    delete_backup(cfg.backup_dir);

    /* �o�b�N�A�b�v�쐬 */
    if(backup_folder(
            cfg.target_dir,
            cfg.backup_dir))
    {
        printf("�o�b�N�A�b�v���s\n");
        return 1;
    }

    /* �t�@�C������ */
    if(organize_files(cfg.target_dir))
    {
        printf("�G���[����\n");
        printf("�����J�n\n");

        /* �����r���̃t�H���_���폜 */
        delete_backup(cfg.target_dir);

        /* �o�b�N�A�b�v���畜�� */
        restore_folder(
            cfg.backup_dir,
            cfg.target_dir
        );

        return 1;
    }

    printf("��������\n");

    return 0;
}

/*==================================================
backup_start()

�c���Ă���o�b�N�A�b�v���畜������B

�߂�l
    0 : ����
    1 : ���s

==================================================*/
int backup_start(void)
{
    Config cfg;

    if(load_config(&cfg))
    {
        return 1;
    }

    struct stat st;

    /* �o�b�N�A�b�v�����݂��Ȃ� */
    if(stat(cfg.backup_dir, &st) != 0)
    {
        printf("�o�b�N�A�b�v�͑��݂��܂���\n");
        return 1;
    }

    printf("�o�b�N�A�b�v�𔭌�\n");
    printf("�����J�n\n");

    /* ���݂̐����ς݃t�H���_�폜 */
    delete_backup(cfg.target_dir);

    /* �o�b�N�A�b�v���畜�� */
    if(restore_folder(
            cfg.backup_dir,
            cfg.target_dir))
    {
        printf("�������s\n");
        return 1;
    }

    printf("��������\n");

    return 0;
}