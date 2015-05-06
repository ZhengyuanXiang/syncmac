#ifndef _DIR_MONITOR_H_
#define _DIR_MONITOR_H_

#include "dir.h"
#include <sys/types.h>


void new_event(char event, void* data);
int chl_dir_change(DIR_NODE *old_dir, DIR_NODE *new_dir);

int file_change(DIR_NODE *old_dir, DIR_NODE *new_dir);
int dir_changes(DIR_NODE *old_dir, DIR_NODE *new_dir);
void monitor();

#endif