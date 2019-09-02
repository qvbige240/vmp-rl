/**
 * History:
 * ================================================================
 * 2019-09-02 qing.zou created
 *
 */

#ifndef CLNT_VMON_H
#define CLNT_VMON_H

//#include "rpc_typedef.h"
#include "rpc_clnt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ClntVmonReq
{
    unsigned int            id;

    void                    *ctx;
    vmp_callback_func       pfn_callback;
} ClntVmonReq;

typedef struct _ClntVmonRsp
{
    unsigned int            id;
    unsigned int            count;
} ClntVmonRsp;

int rpc_clnt_vmon(vmp_rpclnt_t *clnt, ClntVmonReq *info);

#ifdef __cplusplus
}
#endif

#endif // CLNT_VMON_H
