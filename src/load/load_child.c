/**
 * History:
 * ================================================================
 * 2019-08-16 qing.zou created
 *
 */

#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "load_child.h"


int load_child_start(int argc, char *argv[])
{
    pid_t pid = 0;

    VMP_LOGI("start vmp-sa");
    pid = vmp_run_process(VMP_SA, argv);
    VMP_LOGI("started vmp-sa ");
    
    return 0;
}
