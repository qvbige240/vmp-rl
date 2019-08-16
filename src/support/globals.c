
/**
 * History:
 * ================================================================
 * 2019-08-14 qing.zou created
 *
 */

#include "globals.h"

struct globals
{
    void*       log;
    void*       config;
    void*       load;      /* load core */
} g_globals;

void* log_default_config(void)
{
    return g_globals.log;
}

void log_set_config(void *config)
{
    g_globals.log = config;
}

void* global_default_config(void)
{
    return g_globals.config;
}

void global_set_config(void *config)
{
    g_globals.config = config;
}

void* global_default_load(void)
{
    return g_globals.load;
}

void global_set_load(void *load)
{
    g_globals.load = load;
}
