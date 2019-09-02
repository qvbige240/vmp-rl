/**
 * History:
 * ================================================================
 * 2019-08-16 qing.zou created
 *
 */

#ifndef LOAD_VMON_H
#define LOAD_VMON_H

#include "load_typedef.h"

VMP_BEGIN_DELS

typedef struct _LoadVmonReq
{
    unsigned int            id;

    void                    *svc;
    void                    *process;

    void                    *ctx;
    vmp_callback_func       pfn_callback;
} LoadVmonReq;

typedef struct _LoadVmonRsp
{
    unsigned int            num;
} LoadVmonRsp;

void *load_vmon_create(void *parent, LoadVmonReq *req);

int load_vmon_start(void *p);

void load_vmon_done(void *p);

VMP_END_DELS

#endif // LOAD_VMON_H
