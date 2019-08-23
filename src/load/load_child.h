/**
 * History:
 * ================================================================
 * 2019-08-16 qing.zou created
 *
 */

#ifndef LOAD_CHILD_H
#define LOAD_CHILD_H

#include "load_typedef.h"

VMP_BEGIN_DELS

#define APP_VMP_SA      BIN_DEFAULT_PATH"vmp-sa"

typedef struct _LoadChildReq
{
    unsigned int            id;

    void                    *ctx;
    vmp_callback_func       pfn_callback;
} LoadChildReq;

typedef struct _LoadChildRsp
{
    unsigned int            num;
} LoadChildRsp;

void *load_child_create(void *parent, LoadChildReq *req);

int load_child_start(void *p);

void load_child_done(void *p);

VMP_END_DELS

#endif // LOAD_CHILD_H
