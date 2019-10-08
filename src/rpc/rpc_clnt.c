/**
 * History:
 * ================================================================
 * 2019-08-19 qing.zou created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "event2/thread.h"

#include "rpc_clnt.h"
#include "pbc/pbrpc-clnt.h"

typedef struct _PrivInfo
{
    RpclntReq               req;

    int                     cond;
	bool                    initialized;

	pthread_t               thread;
    pthread_mutex_t         mutex;

    //pbrpc_clnt*             clnt;
} PrivInfo;

vmp_rpclnt_t* vmp_rpclnt_create(void)
{
    //pbrpc_clnt *clnt = NULL;
    vmp_rpclnt_t *thiz = calloc(1, sizeof(vmp_rpclnt_t) + sizeof(PrivInfo));
    if(thiz != NULL)
    {
        DECL_PRIV(thiz, priv);
        // if (base)
        //     thiz->event_base = base;

        // clnt = pbrpc_clnt_new("localhost", 9876);
        // if (!clnt) {
        //     fprintf(stderr, "Failed to create an pbrpc_clnt object\n");
        //     goto err;
        // }

        priv->cond  = 1;
        priv->initialized = true;
        //priv->clnt = clnt;
    }

    return thiz;

// err:
//     if (clnt)
//         pbrpc_clnt_destroy(clnt);
//     if (thiz)
//         free(thiz);

//     return NULL;
}

int vmp_rpclnt_set(vmp_rpclnt_t* thiz, void* data)
{
    DECL_PRIV(thiz, priv);
    priv->req = *((RpclntReq*)data);

    return 0;
}

static int rpc_clnt_run(vmp_rpclnt_t* thiz)
{
    if (!thiz) {
        fprintf(stderr, "Failed to run clnt.");
        return -1;
    }

    //DECL_PRIV(thiz, priv);
    
    //pbrpc_clnt *clnt = priv->clnt;
    //event_base_dispatch(bufferevent_get_base(clnt->bev));
    pbrpc_clnt_mainloop(thiz->clnt);

    return 0;
}

static void* rpc_clnt_thread(void* arg)
{
    vmp_rpclnt_t* thiz = (vmp_rpclnt_t*)arg;
    DECL_PRIV(thiz, priv);
    evthread_use_pthreads();

    if (priv->req.base)
        thiz->event_base = priv->req.base;

    thiz->clnt = pbrpc_clnt_new("localhost", 9876);
    //thiz->clnt = pbrpc_clnt_new(priv->req.ip, priv->req.port);
    if (!thiz->clnt) {
        fprintf(stderr, "Failed to create an pbrpc_clnt object\n");
        goto err;
    }

    rpc_clnt_run(thiz);

err:
    return NULL;
}

int vmp_rpclnt_start(vmp_rpclnt_t* thiz)
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

    ret = pthread_create(&priv->thread, NULL, rpc_clnt_thread, (void*)thiz);
    if (ret != 0) {
        RPC_LOGE("create thread 'rpc_clnt_thread' failed");
        goto fail;
    }
    
    return 0;

fail:
    vmp_rpclnt_destroy(thiz);
    return -1;
}

static int rpc_clnt_stop(PrivInfo *thiz)
{
    void *thread_result;

    if (!thiz) return -1;

    thiz->cond = 0;
    if (thiz->initialized) {
        thiz->initialized = false;
        pthread_join(thiz->thread, &thread_result);
    }

    return 0;
}

int vmp_rpclnt_destroy(vmp_rpclnt_t* thiz)
{
    DECL_PRIV(thiz, priv);
    if (thiz)
    {
        if (thiz->clnt)
            pbrpc_clnt_destroy(thiz->clnt);

        rpc_clnt_stop(priv);
        free(thiz);
    }

    return 0;
}

// extern int workload_call(pbrpc_clnt *clnt);
// int rpc_workload_call(vmp_rpclnt_t* thiz)
// {
//     //DECL_PRIV(thiz, priv);
//     if (thiz && thiz->clnt)
//     {
//         return workload_call(thiz->clnt);
//     }

//     return 0;
// }
