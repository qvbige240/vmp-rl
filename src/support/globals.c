
/**
 * History:
 * ================================================================
 * 2019-08-14 qing.zou created
 *
 */

#include "globals.h"

struct globals
{
    void    *log;
} g_globals;

void* log_default_config(void)
{
    return g_globals.log;
}

void log_set_config(void *config)
{
    g_globals.log = config;
}
