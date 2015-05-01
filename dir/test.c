#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h> 
#include <string.h> 

#define DIR_MAX 20
#define FILE_MAX 30
#define ERR 1
#define OK 0
#define DIR_LEN_MAX 300

#define DIRNODE SIF_DIR
#define FILENODE SIF_REG

typedef struct tag_file_node
{
    char *relate_name;
    char *name;
    struct tag_file_node *next_file;
}FILE_NODE;

typedef struct tag_dir_node
{
    char *relate_name;
    char *name;
    struct tag_dir_node *next_dir;
    struct tag_file_node *next_file;
}DIR_NODE;

DIR_NODE * get_a_new_dir_node(char *relate_name, char *name);
int read_all_dirent(DIR_NODE *dir_node);
FILE_NODE * get_a_new_file_node(char *relate_name, char *name);
void inset_a_dir(DIR_NODE *work_dir, DIR_NODE *new_dir);
void inset_a_file(DIR_NODE *work_dir, FILE_NODE *new_file);

void print_dir(DIR_NODE *dir_node)
{
    FILE_NODE * file_node = dir_node->next_file;
    DIR_NODE * work_dir_node = dir_node;
    printf("-dir %s\n", dir_node->relate_name);
    while(file_node)
    {
        printf("-reg %s\n", file_node->name);
        file_node = file_node->next_file;
    }
    while (work_dir_node->next_dir)
    {
        print_dir(work_dir_node->next_dir);
        work_dir_node = work_dir_node->next_dir;
    }
}

void inset_a_dir(DIR_NODE *work_dir, DIR_NODE *new_dir)
{
    DIR_NODE *tmp_dir_node = work_dir;
    //printf("insert dir %s %s\n", work_dir->relate_name, new_dir->name);
    while(tmp_dir_node->next_dir)
        tmp_dir_node = tmp_dir_node->next_dir;
    tmp_dir_node->next_dir = new_dir;
}
void inset_a_file(DIR_NODE *work_dir, FILE_NODE *new_file)
{
    FILE_NODE *tmp_file_node = work_dir->next_file;
    //printf("insert file %s %s\n", work_dir->relate_name, new_file->name);

    if (tmp_file_node == NULL)
    {
        work_dir->next_file = new_file;
        return;
    }
    while(tmp_file_node->next_file)
        tmp_file_node = tmp_file_node->next_file;
    tmp_file_node->next_file = new_file;
    return;
}
int read_all_dirent(DIR_NODE *dir_node)
{
    DIR *dirp = NULL;
    struct dirent *direntp;
    struct stat statbuf;
    char file_path[DIR_LEN_MAX] = {};
    DIR_NODE *chld_dir_node;
    FILE_NODE *chld_file_node;

    if (NULL == (dirp = opendir(dir_node->relate_name)))
    {
        printf("open dir failed %d %s\n", errno, dir_node->relate_name);
        return ERR;
    }
   
    while(NULL != (direntp = readdir(dirp)))
    {
        if (strcmp(direntp->d_name, ".") == 0 || 
                  strcmp(direntp->d_name, "..") == 0)
            continue;
        memset(file_path, 0, sizeof(file_path));
        strcpy(file_path, dir_node->relate_name);
        strcat(file_path, "/");
        strcat(file_path, direntp->d_name);
        if (lstat(file_path, &statbuf) < 0)
        {
            printf("lstat %s error %d\n", file_path, errno);
            continue;
        }
        if (S_ISDIR(statbuf.st_mode))
        {
            //printf("dir %s\n", file_path);
            chld_dir_node = get_a_new_dir_node(file_path, direntp->d_name);
            inset_a_dir(dir_node, chld_dir_node);

            read_all_dirent(chld_dir_node);
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            chld_file_node = get_a_new_file_node(file_path, direntp->d_name);
            inset_a_file(dir_node, chld_file_node);
            //printf("reg %s\n", file_path);
        }
        else
        {
            printf("unknow %s\n", file_path);
        }
    }
    closedir(dirp);
    return OK;
    
}

DIR_NODE * get_a_new_dir_node(char *relate_name, char *name)
{
    DIR_NODE *dir_node = malloc(sizeof(DIR_NODE));

    dir_node->relate_name = malloc(strlen(relate_name) + 1);
    memset(dir_node->relate_name, 0, strlen(relate_name) + 1);
    strcpy(dir_node->relate_name, relate_name);

    dir_node->name = malloc(strlen(name) + 1);
    memset(dir_node->name, 0, strlen(name) + 1);
    strcpy(dir_node->name, name);

    dir_node->next_dir = NULL;
    dir_node->next_file = NULL;

    return dir_node;
}

FILE_NODE * get_a_new_file_node(char *relate_name, char *name)
{
    FILE_NODE *file_node = malloc(sizeof(FILE_NODE));

    file_node->relate_name = malloc(strlen(relate_name) + 1);
    memset(file_node->relate_name, 0, strlen(relate_name) + 1);
    strcpy(file_node->relate_name, relate_name);

    file_node->name = malloc(strlen(name) + 1);
    memset(file_node->name, 0, strlen(name) + 1);
    strcpy(file_node->name, name);

    file_node->next_file = NULL;

    return file_node;
}

int main(int argc, char **args)
{
    DIR_NODE *program_root_node = get_a_new_dir_node(".", ".");

    read_all_dirent(program_root_node);
    printf("--------------------------\n");
    printf("--------------------------\n");
    printf("--------------------------\n");
    printf("--------------------------\n");
    sleep(1);
    print_dir(program_root_node);
}