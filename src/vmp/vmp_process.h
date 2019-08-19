/**
 * History:
 * ================================================================
 * 2019-08-16 qing.zou created
 *
 */

#ifndef VMP_PROCESS_H
#define VMP_PROCESS_H

#include "vmp_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

int vmp_run_process(const char* cmd, char* argv[]);
int vmp_waitpid(pid_t pid);

#ifdef __cplusplus
}
#endif

#endif // VMP_PROCESS_H
