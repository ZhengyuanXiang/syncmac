#include "dir.h"
#include "syncmac.h"
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

    //PRINT("free dir %s\n", dir->name);
    free(dir->name);

    free(dir);
}

int is_same_file(FILE_NODE *old_file, FILE_NODE *new_file)
{
    char old_file_path[DIR_LEN_MAX] = {0};
    char new_file_path[DIR_LEN_MAX] = {0};

    get_file_full_name(old_file, old_file_path);
    get_file_full_name(new_file, new_file_path);
    if (0 == strcmp(old_file_path, new_file_path))
        return OK;
    return ERR;
}

int is_same_dir(DIR_NODE *old_dir, DIR_NODE *new_dir)
{
    char old_file_path[DIR_LEN_MAX] = {0};
    char new_file_path[DIR_LEN_MAX] = {0};

    get_dir_full_name(old_dir, old_file_path);
    get_dir_full_name(new_dir, new_file_path);
    if (0 == strcmp(old_file_path, new_file_path))
        return OK;
    return ERR;
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
    //PRINT("free file %s\n", file->name);
    free(file->name);
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
    //PRINT("-dir %s\n", dir->full_name);
    while(file)
    {
        //PRINT("-reg %s\n", file->name);
        file = file->next_file;
    }
    while (chl_dir)
    {
        print_dir(chl_dir);
        chl_dir = chl_dir->next_bro_dir;
    }
}

void insert_a_chl_dir(DIR_NODE *parent_dir, DIR_NODE *new_dir)
{
    DIR_NODE *chl_dir = parent_dir->next_chl_dir;

    new_dir->parent_dir = parent_dir;

    if (chl_dir == NULL)
    {
        parent_dir->next_chl_dir = new_dir;
        return;
    }
    while(chl_dir->next_bro_dir)
        chl_dir = chl_dir->next_bro_dir;
    chl_dir->next_bro_dir = new_dir;
}
void insert_a_file(DIR_NODE *parent_dir, FILE_NODE *new_file)
{
    FILE_NODE *tmp_file_node = parent_dir->next_file;
    //PRINT("insert file %s %s\n", parent_dir->full_name, new_file->name);

    new_file->parent_dir = parent_dir;

    if (tmp_file_node == NULL)
    {
        parent_dir->next_file = new_file;
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

void get_dir_full_name(DIR_NODE* dir, char *name)
{
    if (dir->parent_dir != NULL)
    {
        get_dir_full_name(dir->parent_dir, name);
        strcat(name, "/");
    }
    strcat(name, dir->name);
}

void get_file_full_name(FILE_NODE* file, char *name)
{
    if (file->parent_dir != NULL)
    {
        get_dir_full_name(file->parent_dir, name);
        strcat(name, "/");
    }
    strcat(name, file->name);
}

int read_all_dirent(DIR_NODE *dir)
{
    DIR *dirp = NULL;
    struct dirent *direntp;
    struct stat statbuf;
    char file_path[DIR_LEN_MAX] = {0};
    char parent_dir_full_name[DIR_LEN_MAX] = {0};
    DIR_NODE *chld_dir_node;
    FILE_NODE *chld_file_node;

    get_dir_full_name(dir, parent_dir_full_name);

    if (NULL == (dirp = opendir(parent_dir_full_name)))
    {
        PRINT("open dir failed %d %s\n", errno, parent_dir_full_name);
        return ERR;
    }
   
    while(NULL != (direntp = readdir(dirp)))
    {
        if (OK == filter(direntp->d_name))
            continue;
        memset(file_path, 0, sizeof(file_path));
        strcpy(file_path, parent_dir_full_name);
        strcat(file_path, "/");
        strcat(file_path, direntp->d_name);
        if (lstat(file_path, &statbuf) < 0)
        {
            PRINT("lstat %s error %d\n", file_path, errno);
            continue;
        }
        if (S_ISDIR(statbuf.st_mode))
        {
            //PRINT("dir %s\n", file_path);
            chld_dir_node = get_a_new_dir_node(direntp->d_name);
            insert_a_chl_dir(dir, chld_dir_node);

            read_all_dirent(chld_dir_node);
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            chld_file_node = get_a_new_file_node(direntp->d_name);
            insert_a_file(dir, chld_file_node);
            //PRINT("reg %s\n", file_path);
        }
        else
        {
            //PRINT("unknow %s\n", file_path);
        }
    }
    closedir(dirp);
    return OK;
    
}

DIR_NODE * get_a_new_dir_node(char *name)
{
    DIR_NODE *dir = malloc(sizeof(DIR_NODE));

    dir->name = malloc(strlen(name) + 1);
    memset(dir->name, 0, strlen(name) + 1);
    strcpy(dir->name, name);

    dir->next_bro_dir = NULL;
    dir->next_chl_dir = NULL;
    dir->next_file = NULL;
    dir->parent_dir = NULL;

    return dir;
}

FILE_NODE * get_a_new_file_node(char *name)
{
    FILE_NODE *file = malloc(sizeof(FILE_NODE));

    file->name = malloc(strlen(name) + 1);
    memset(file->name, 0, strlen(name) + 1);
    strcpy(file->name, name);

    file->next_file = NULL;
    file->parent_dir = NULL;
    return file;
}