/**
 * History:
 * ================================================================
 * 2019-08-19 qing.zou created
 *
 */

#ifndef RPC_CLNT_H
#define RPC_CLNT_H

#include "rpc_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _RpclntReq
{
	void*					base;

    char                    ip[64];
	unsigned short			port;

	void*					ctx;
	//bufferevent_data_cb		read_cb;
} RpclntReq;

typedef struct vmp_rpclnt_t
{
    void*           core;
    void*           event_base;

    char            priv[ZERO_LEN_ARRAY];
} vmp_rpclnt_t;

vmp_rpclnt_t* vmp_rpclnt_create(void);
int vmp_rpclnt_set(vmp_rpclnt_t* thiz, void* data);
int vmp_rpclnt_start(vmp_rpclnt_t* thiz);
int vmp_rpclnt_destroy(vmp_rpclnt_t* thiz);

int rpc_workload_call(vmp_rpclnt_t* thiz);

#ifdef __cplusplus
}
#endif

#endif // RPC_CLNT_H
