/**
 * History:
 * ================================================================
 * 2019-08-16 qing.zou created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "rpc_svc.h"
#include "pbc/pbrpc-svc.h"
#include "service/service-gather.h"


typedef struct _PrivInfo
{
    RpcsvcReq               req;
    
    int                     cond;

	pthread_t               thread;
    pthread_mutex_t         svc_mutex;
	// os_event_t                 *stop_event;

	bool                    initialized;

    pbrpc_svc*              svc;
} PrivInfo;

static pbrpc_svc_callout method_table[] =
{
    { 200000, 1, "Loader.workload", workload_exec },
    { 200001, 1, "Loader.registry", registry_exec },
};

vmp_rpcsvc_t* vmp_rpcsvc_create(void)
{
    //int ret = 0;
    //pbrpc_svc *svc = NULL;
    vmp_rpcsvc_t *thiz = calloc(1, sizeof(vmp_rpcsvc_t) + sizeof(PrivInfo));
    if (thiz != NULL)
    {
        DECL_PRIV(thiz, priv);

        // svc = pbrpc_svc_new("localhost", 9876);
        // if (!svc) {
        //     fprintf(stderr, "Failed to create a new pbrpc_svc object");
        //     goto err;
        // }
        // ret = pbrpc_svc_register_methods(svc, method_table);
        // if (ret) {
        //     fprintf(stderr, "Failed to register methods to pbrpc_svc.");
        //     goto err;
        // }

        priv->cond = 1;
        priv->initialized = true;
        //priv->svc = svc;
    }

    return thiz;

// err:
//     if (svc)
//         pbrpc_svc_destroy(svc);
//     if (thiz)
//         free(thiz);
//     return NULL;
}

int vmp_rpcsvc_set(vmp_rpcsvc_t* thiz, void* data)
{
    DECL_PRIV(thiz, priv);
    priv->req = *((RpcsvcReq*)data);

    return 0;
}

static int rpc_svc_run(vmp_rpcsvc_t* thiz)
{
    int ret = 0;
    if (!thiz) {
        fprintf(stderr, "Failed to run svc.");
        return -1;
    }

    DECL_PRIV(thiz, priv);

    ret = pbrpc_svc_run(priv->svc);
    if (ret) {
        fprintf(stderr, "Failed to start libevent base loop for "
                        "pbrpc_svc listener.");
        return -1;
    }

    return 0;
}

static void* rpc_svc_thread(void* arg)
{
    int ret = 0;
    vmp_rpcsvc_t* thiz = (vmp_rpcsvc_t*)arg;
    DECL_PRIV(thiz, priv);
    //evthread_use_pthreads();

    //svc = pbrpc_svc_new("localhost", 9876);
    priv->svc = pbrpc_svc_new("localhost", priv->req.port);
    if (!priv->svc) {
        fprintf(stderr, "Failed to create a new pbrpc_svc object");
        goto err;
    }

    ret = pbrpc_svc_register_methods(priv->svc, method_table);
    if (ret) {
        fprintf(stderr, "Failed to register methods to pbrpc_svc.");
        goto err;
    }

    rpc_svc_run(thiz);    

err:
    //vmp_rpcsvc_destroy(thiz);
    return NULL;
}

int vmp_rpcsvc_start(vmp_rpcsvc_t* thiz)
{
    int ret = 0;
    //pthread_mutexattr_t attr;
    DECL_PRIV(thiz, priv);

    if (!thiz) return -1;
    
    // if (pthread_mutexattr_init(&attr) != 0)
    //     goto fail;
    // if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
    //     goto fail;
    // if (pthread_mutex_init(&thiz->core_mutex, &attr) != 0)
    //     goto fail;

    ret = pthread_create(&priv->thread, NULL, rpc_svc_thread, (void*)thiz);
    if (ret != 0) {
        RPC_LOGE("create thread 'rpc_svc_thread' failed");
        goto fail;
    }

    return 0;

fail:
    vmp_rpcsvc_destroy(thiz);
    return -1;
}

static int rpc_svc_stop(PrivInfo *thiz)
{
    void *thread_result;

    if (!thiz)
        return -1;

    thiz->cond = 0;
    if (thiz->initialized) {
        thiz->initialized = false;
        pthread_join(thiz->thread, &thread_result);
    }

    return 0;
}

int vmp_rpcsvc_destroy(vmp_rpcsvc_t* thiz)
{
    DECL_PRIV(thiz, priv);
    if (thiz)
    {
        if (priv->svc)
            pbrpc_svc_destroy(priv->svc);

        rpc_svc_stop(priv);
        free(thiz);
    }

    return 0;
}
