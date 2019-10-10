/**
 * History:
 * ================================================================
 * 2019-10-08 qing.zou created
 *
 */

#ifndef CLNT_NODE_STATE_H
#define CLNT_NODE_STATE_H

#include "rpc_clnt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ClntNodeStateReq
{
    unsigned int            id;

    void                    *ctx;
    vmp_callback_func       pfn_callback;
} ClntNodeStateReq;

typedef struct _ClntNodeStateRsp
{
    unsigned int            id;
    unsigned int            index;
    char                    *name;
    unsigned int            count;
    unsigned long           uplink;
    unsigned long           downlink;
    unsigned long           memory;
    double                  cpu;
} ClntNodeStateRsp;

int rpc_clnt_node_state(vmp_rpclnt_t *clnt, ClntNodeStateReq *info);

#ifdef __cplusplus
}
#endif

#endif // CLNT_NODE_STATE_H
