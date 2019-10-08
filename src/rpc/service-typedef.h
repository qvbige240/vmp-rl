/**
 * History:
 * ================================================================
 * 2019-08-29 qing.zou created
 *
 */

#ifndef SERVICE_TYPEDEF_H
#define SERVICE_TYPEDEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RPC_SERVICE_PROGRAM             200000
#define RPC_SERVICE_PROGRAM_DEMO        200001
#define RPC_SERVICE_PROGRAM_REGISTRY    200002
#define RPC_SERVICE_PROGRAM_VMON        200003

typedef int (*rpc_service_func)(void *p, void *req, void *rsp);

typedef struct svc_handler_s
{
    void               *ctx;
    rpc_service_func    pfn_callback;
} svc_handler_t;


#ifdef __cplusplus
}
#endif

#endif // SERVICE_TYPEDEF_H
