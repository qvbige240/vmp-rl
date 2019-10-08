/**
 * History:
 * ================================================================
 * 2019-10-06 qing.zou created
 *
 */

#ifndef SERVICE_DEMO_H
#define SERVICE_DEMO_H

#include "service-typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEMO_SC_PROG		RPC_SERVICE_PROGRAM_DEMO    //((u_long)100000)
#define DEMO_SC_VERS		(1)
#define DEMO_SC_NAME		"Loader.demo"

typedef struct _ServiceDemoReq
{
    unsigned int            id;
} ServiceDemoReq;

typedef struct _ServiceDemoRsp
{
    unsigned int            cnt;
    unsigned long           mem;
} ServiceDemoRsp;

int service_demo_register(void *svc, void *args);


#ifdef __cplusplus
}
#endif

#endif // SERVICE_DEMO_H
