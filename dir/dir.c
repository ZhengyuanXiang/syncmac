#include "dir.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void free_dir(DIR_NODE *dir)
{
    FILE_NODE *file_del = dir->next_file;
    FILE_NODE *file_del_next;
    DIR_NODE *del_chl_dir = dir->next_chl_dir;
    DIR_NODE *next_del_chl_dir;

    while (file_del)
    {
        file_del_next = file_del->next_file;
        free_file(file_del);
        file_del = file_del_next;
    }

    while (del_chl_dir)
    {
        next_del_chl_dir = del_chl_dir->next_bro_dir;
        free_dir(del_chl_dir);
        del_chl_dir = next_del_chl_dir;
    }

    printf("free dir %s\n", dir->name);
    free(dir->name);
    free(dir->full_name);
    free(dir);
}

int remove_a_dir(DIR_NODE *curr_dir, char *dir_name)
{
    DIR_NODE *del_dir = curr_dir->next_chl_dir;
    DIR_NODE *pre_del_dir;

    if (del_dir == NULL)
        return ERR;
    if (0 == strcmp(del_dir->name, dir_name))
    {
        curr_dir->next_chl_dir = del_dir->next_bro_dir;
        free_dir(del_dir);
        return OK;
    }

    pre_del_dir = del_dir;
    del_dir = del_dir->next_bro_dir;
    while (del_dir != NULL)
    {
        if (0 == strcmp(del_dir->name, dir_name))
        {
            pre_del_dir = del_dir->next_bro_dir;
            free_dir(del_dir);
            return OK;
        }
        pre_del_dir = del_dir;
        del_dir = del_dir->next_bro_dir;
    }
    return ERR;
}

void free_file(FILE_NODE *file)
{
    printf("free file %s\n", file->name);
    free(file->name);
    free(file->full_name);
    free(file);
}

int remove_a_file(DIR_NODE *curr_dir, char *file_name)
{
    FILE_NODE *del_file = curr_dir->next_file;
    FILE_NODE *pre_del_file;
 
    if (del_file == NULL)
        return ERR;
    if (0 == strcmp(del_file->name, file_name))
    {
        curr_dir->next_file = del_file->next_file;
        free_file(del_file);
        return OK;
    }
    pre_del_file = del_file;
    del_file = del_file->next_file;
    while (del_file != NULL)
    {
        if (0 == strcmp(del_file->name, file_name))
        {
            pre_del_file->next_file = del_file->next_file;
            free_file(del_file);
            return OK;
        }
        pre_del_file = del_file;
        del_file = del_file->next_file;
    }
    return ERR;
}
void print_dir(DIR_NODE *dir)
{
    FILE_NODE * file = dir->next_file;
    DIR_NODE * chl_dir = dir->next_chl_dir;
    printf("-dir %s\n", dir->full_name);
    while(file)
    {
        printf("-reg %s\n", file->name);
        file = file->next_file;
    }
    while (chl_dir)
    {
        print_dir(chl_dir);
        chl_dir = chl_dir->next_bro_dir;
    }
}

void insert_a_chl_dir(DIR_NODE *curr_dir, DIR_NODE *new_dir)
{
    DIR_NODE *chl_dir = curr_dir->next_chl_dir;
    if (chl_dir == NULL)
    {
        curr_dir->next_chl_dir = new_dir;
        return;
    }
    while(chl_dir->next_bro_dir)
        chl_dir = chl_dir->next_bro_dir;
    chl_dir->next_bro_dir = new_dir;
}
void insert_a_file(DIR_NODE *curr_dir, FILE_NODE *new_file)
{
    FILE_NODE *tmp_file_node = curr_dir->next_file;
    //printf("insert file %s %s\n", curr_dir->full_name, new_file->name);

    if (tmp_file_node == NULL)
    {
        curr_dir->next_file = new_file;
        return;
    }
    while(tmp_file_node->next_file)
        tmp_file_node = tmp_file_node->next_file;
    tmp_file_node->next_file = new_file;
    return;
}

int filter(char *name)
{
    if (strcmp(name, ".") == 0 || 
            strcmp(name, "..") == 0 ||
            strcmp(name, "a.out.dSYM") == 0)
        return OK;
    return ERR;
}

int read_all_dirent(DIR_NODE *dir)
{
    DIR *dirp = NULL;
    struct dirent *direntp;
    struct stat statbuf;
    char file_path[DIR_LEN_MAX] = {};
    DIR_NODE *chld_dir_node;
    FILE_NODE *chld_file_node;

    if (NULL == (dirp = opendir(dir->full_name)))
    {
        printf("open dir failed %d %s\n", errno, dir->full_name);
        return ERR;
    }
   
    while(NULL != (direntp = readdir(dirp)))
    {
        if (OK == filter(direntp->d_name))
            continue;
        memset(file_path, 0, sizeof(file_path));
        strcpy(file_path, dir->full_name);
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
            insert_a_chl_dir(dir, chld_dir_node);

            read_all_dirent(chld_dir_node);
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            chld_file_node = get_a_new_file_node(file_path, direntp->d_name);
            insert_a_file(dir, chld_file_node);
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

DIR_NODE * get_a_new_dir_node(char *full_name, char *name)
{
    DIR_NODE *dir = malloc(sizeof(DIR_NODE));

    dir->full_name = malloc(strlen(full_name) + 1);
    memset(dir->full_name, 0, strlen(full_name) + 1);
    strcpy(dir->full_name, full_name);

    dir->name = malloc(strlen(name) + 1);
    memset(dir->name, 0, strlen(name) + 1);
    strcpy(dir->name, name);

    dir->next_bro_dir = NULL;
    dir->next_chl_dir = NULL;
    dir->next_file = NULL;

    return dir;
}

FILE_NODE * get_a_new_file_node(char *full_name, char *name)
{
    FILE_NODE *file = malloc(sizeof(FILE_NODE));

    file->full_name = malloc(strlen(full_name) + 1);
    memset(file->full_name, 0, strlen(full_name) + 1);
    strcpy(file->full_name, full_name);

    file->name = malloc(strlen(name) + 1);
    memset(file->name, 0, strlen(name) + 1);
    strcpy(file->name, name);

    file->next_file = NULL;

    return file;
}

int read_dirent(DIR_NODE *dir)
{
    DIR *dirp = NULL;
    struct dirent *direntp;
    struct stat statbuf;
    char file_path[DIR_LEN_MAX] = {};
    DIR_NODE *chld_dir_node;
    FILE_NODE *chld_file_node;

    if (NULL == (dirp = opendir(dir->full_name)))
    {
        printf("open dir failed %d %s\n", errno, dir->full_name);
        return ERR;
    }
   
    while(NULL != (direntp = readdir(dirp)))
    {
        if (OK == filter(direntp->d_name))
            continue;
        memset(file_path, 0, sizeof(file_path));
        strcpy(file_path, dir->full_name);
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
            insert_a_chl_dir(dir, chld_dir_node);
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            chld_file_node = get_a_new_file_node(file_path, direntp->d_name);
            insert_a_file(dir, chld_file_node);
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