/**
 * History:
 * ================================================================
 * 2019-10-08 qing.zou created
 *
 */

#ifndef SERVICE_SERVER_REGISTRY_H
#define SERVICE_SERVER_REGISTRY_H

#include "service-typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REGISTRY_SC_PROG		RPC_SERVICE_PROGRAM_REGISTRY    //((u_long)100000)
#define REGISTRY_SC_VERS		(1)
#define REGISTRY_SC_NAME		"Loader.registry"

typedef struct _RpcServiceRegistryReq
{
    unsigned int            id;
    char                    name[64];
    char                    system[64];
    char                    location[64];
    char                    processor[64];
    unsigned long           bandwidth;
    unsigned long           memory;

    char                    ip[64];
    unsigned short          port;
} RpcServiceRegistryReq;

typedef struct _RpcServiceRegistryRsp
{
    unsigned int            id;
    char                    name[64];
    unsigned int            index;
    unsigned int            num;
} RpcServiceRegistryRsp;

int service_registry_register(void *svc, void *args);


#ifdef __cplusplus
}
#endif

#endif // SERVICE_SERVER_REGISTRY_H
