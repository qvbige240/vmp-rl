/**
 * History:
 * ================================================================
 * 2019-10-06 qing.zou created
 *
 */

#ifndef CLNT_DEMO_H
#define CLNT_DEMO_H

#include "rpc_clnt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ClntDemoReq
{
    unsigned int            id;

    void                    *ctx;
    vmp_callback_func       pfn_callback;
} ClntDemoReq;

typedef struct _ClntDemoRsp
{
    unsigned int            id;
    unsigned int            count;
    unsigned int            memory;
} ClntDemoRsp;

int rpc_clnt_demo(vmp_rpclnt_t *clnt, ClntDemoReq *info);

#ifdef __cplusplus
}
#endif

#endif // CLNT_DEMO_H
