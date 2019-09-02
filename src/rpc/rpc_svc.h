/**
 * History:
 * ================================================================
 * 2019-08-16 qing.zou created
 *
 */

#ifndef RPC_SVC_H
#define RPC_SVC_H

#include "rpc_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _RpcsvcReq
{
	void*					base;
	unsigned short			port;
	void*					ctx;
	//bufferevent_data_cb		read_cb;
} RpcsvcReq;

typedef struct vmp_rpcsvc_t
{
    void*           core;
    void*           event_base;

    void*           svc;

    char            priv[ZERO_LEN_ARRAY];
} vmp_rpcsvc_t;

vmp_rpcsvc_t* vmp_rpcsvc_create(void);
int vmp_rpcsvc_set(vmp_rpcsvc_t* thiz, void* data);
int vmp_rpcsvc_start(vmp_rpcsvc_t* thiz);
int vmp_rpcsvc_destroy(vmp_rpcsvc_t* thiz);


#ifdef __cplusplus
}
#endif

#endif // RPC_SVC_H
