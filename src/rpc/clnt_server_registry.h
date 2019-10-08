/**
 * History:
 * ================================================================
 * 2019-08-20 qing.zou created
 *
 */

#ifndef RPC_SERVER_REGISTRY_H
#define RPC_SERVER_REGISTRY_H

#include "rpc_clnt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ClntServerRegistryReq
{
    unsigned int            id;
    char                    *name;
    char                    *system;
    char                    *location;
    char                    *processor;
    unsigned long           bandwidth;
    unsigned long           memory;

    char                    ip[64];
    unsigned short          port;

    void                    *ctx;
    vmp_callback_func       pfn_callback;
} ClntServerRegistryReq;

typedef struct _ClntServerRegistryRsp
{
    unsigned int            id;
    char                    *name;
    unsigned int            index;
    unsigned int            num;
} ClntServerRegistryRsp;

int rpc_clnt_registry(vmp_rpclnt_t *clnt, ClntServerRegistryReq *info);

#ifdef __cplusplus
}
#endif

#endif // RPC_SERVER_REGISTRY_H
