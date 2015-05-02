#ifndef _DIR_MONITOR_H_
#define _DIR_MONITOR_H_

#include "dir.h"

void add_dir_change_event(int event, DIR_NODE *dir);
void add_file_change_event(int event, FILE_NODE *file);
void chl_dir_change(DIR_NODE *old_dir, DIR_NODE *new_dir);
int is_same_file(FILE_NODE *old_file, FILE_NODE *new_file);
int is_same_dir(DIR_NODE *old_dir, DIR_NODE *new_dir);
void file_change(DIR_NODE *old_dir, DIR_NODE *new_dir);
void dir_changes(DIR_NODE *old_dir, DIR_NODE *new_dir);

#endif