/**
 * History:
 * ================================================================
 * 2019-08-16 qing.zou created
 *
 */

#ifndef LOAD_DEMO_H
#define LOAD_DEMO_H

#include "load_typedef.h"

VMP_BEGIN_DELS

typedef struct _LoadDemoReq
{
    unsigned int            id;

    void                    *svc;
    void                    *process;

    void                    *ctx;
    vmp_callback_func       pfn_callback;
} LoadDemoReq;

typedef struct _LoadDemoRsp
{
    unsigned int            num;
} LoadDemoRsp;

void *load_demo_create(void *parent, LoadDemoReq *req);

int load_demo_start(void *p);

void load_demo_done(void *p);

VMP_END_DELS

#endif // LOAD_DEMO_H
