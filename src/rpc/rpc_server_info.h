/**
 * History:
 * ================================================================
 * 2019-08-20 qing.zou created
 *
 */

#ifndef RPC_SERVERINFO_H
#define RPC_SERVERINFO_H

#include "rpc_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _RpcServerInfoReq
{
	unsigned int            id;
    char*                   name;
    char*                   system;
    char*                   location;
    unsigned long int       bandwidth;

    char                    ip[64];
	unsigned short			port;

	void*					ctx;
} RpcServerInfoReq;

typedef struct _RpcServerInfoRsp
{
	unsigned int            id;
    char*                   name;
    char*                   ip;
    unsigned int            num;
} RpcServerInfoRsp;

int rpc_call_registry(void* clnt, RpcServerInfoReq *info);


#ifdef __cplusplus
}
#endif

#endif // RPC_SERVERINFO_H
