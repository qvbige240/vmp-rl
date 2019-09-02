/**
 * History:
 * ================================================================
 * 2019-08-29 qing.zou created
 *
 */

#ifndef SERVICE_VMON_H
#define SERVICE_VMON_H

#include "service-typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VMON_SC_PROG		RPC_SERVICE_PROGRAM_VMON    //((u_long)100000)
#define VMON_SC_VERS		(1)
#define VMON_SC_NAME		"Loader.vmon"

typedef struct _ServiceVmonReq
{
    unsigned int            id;
} ServiceVmonReq;

typedef struct _ServiceVmonRsp
{
    unsigned int            num;
} ServiceVmonRsp;

int service_vmon_register(void *svc, void *args);


#ifdef __cplusplus
}
#endif

#endif // SERVICE_VMON_H
