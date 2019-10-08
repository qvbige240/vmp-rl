/**
 * History:
 * ================================================================
 * 2019-08-15 qing.zou created
 *
 */

#ifndef LOAD_CORE_H
#define LOAD_CORE_H

#include "load_typedef.h"

#include "load_child.h"
#include "load_demo.h"
#include "load_vmon.h"

VMP_BEGIN_DELS

typedef struct rt_load_t
{
    void    *core;
} rt_load_t;


void load_core_init(void);
void load_core_done(void);

VMP_END_DELS

#endif // LOAD_CORE_H
