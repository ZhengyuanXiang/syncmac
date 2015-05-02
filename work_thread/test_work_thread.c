#include "work_thread.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **args)
{
    init_work_thr_pool();
    pause();
}