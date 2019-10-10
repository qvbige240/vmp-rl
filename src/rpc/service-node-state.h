/**
 * History:
 * ================================================================
 * 2019-10-08 qing.zou created
 *
 */

#ifndef SERVICE_NODE_STATE_H
#define SERVICE_NODE_STATE_H

#include "service-typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NODE_STATE_SC_PROG		RPC_SERVICE_PROGRAM_NODE_STATE    //((u_long)100000)
#define NODE_STATE_SC_VERS		(1)
#define NODE_STATE_SC_NAME		"Loader.node_state"

typedef struct _ServiceNodeStateReq
{
    unsigned int            id;
} ServiceNodeStateReq;

typedef struct _ServiceNodeStateRsp
{
    unsigned int            id;
    unsigned int            index;
    char                    *name;
    unsigned int            count;
    unsigned long           uplink;
    unsigned long           downlink;
    unsigned long           memory;
    double                  cpu;
} ServiceNodeStateRsp;

int service_node_state_register(void *svc, void *args);


#ifdef __cplusplus
}
#endif

#endif // SERVICE_NODE_STATE_H
