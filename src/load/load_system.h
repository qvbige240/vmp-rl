/**
 * History:
 * ================================================================
 * 2019-08-22 qing.zou created
 *
 */

#ifndef LOAD_SYSTEM_H
#define LOAD_SYSTEM_H

#include "load_typedef.h"

VMP_BEGIN_DELS

typedef struct _LoadSystemReq
{
    unsigned int            id;

    void                    *ctx;
    vmp_callback_func       pfn_callback;
} LoadSystemReq;

typedef struct _LoadSystemRsp
{

} LoadSystemRsp;

void *load_system_create(void *parent, LoadSystemReq *req);
int load_system_start(void *p);

int load_system_interval_set(void *p, int interval);

double ls_uplink_get(void *p);
double ls_downlink_get(void *p);

VMP_END_DELS

#endif // LOAD_SYSTEM_H
