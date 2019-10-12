/**
 * History:
 * ================================================================
 * 2019-10-11 qing.zou created
 *
 */

#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "load_system.h"

typedef struct _PrivInfo
{
    LoadSystemReq           req;
    LoadSystemRsp           rsp;

    int                     interval;

    int                     cond;

    bool                    initialized;

    pthread_t               thread;
    pthread_mutex_t         mutex;
} PrivInfo;

static void load_system_test(PrivInfo *thiz)
{
    VMP_LOGD("load_system_test run test");
}

static void *load_system_thread(void *arg)
{
    PrivInfo *thiz = (PrivInfo *)arg;

    load_system_test(thiz);

    while (thiz->cond)
    {
        sleep(5);
    }

    pthread_exit(0);

    return NULL;
}

int load_system_start(void *p)
{
    int ret = -1;
    pthread_mutexattr_t attr;
    PrivInfo *thiz = p;

    if (!thiz) return -1;

    if (pthread_mutexattr_init(&attr) != 0)
        goto fail;
    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
        goto fail;
    if (pthread_mutex_init(&thiz->mutex, &attr) != 0)
        goto fail;

    ret = pthread_create(&thiz->thread, NULL, load_system_thread, (void *)thiz);
    if (ret != 0)
        VMP_LOGE("create thread 'load_system_thread' failed");

    pthread_detach(thiz->thread);

fail:
    //load_system_done(thiz);
    return ret;
}

int load_system_stop(PrivInfo *thiz)
{
    void *thread_result;

    if (!thiz) return -1;

    thiz->cond = 0;

    if (thiz->initialized)
    {
        thiz->initialized = false;
        //pthread_join(thiz->thread, &thread_result);
    }

    return 0;
}

void *load_system_create(void *parent, LoadSystemReq *req)
{
    PrivInfo *priv = calloc(1, sizeof(PrivInfo));
    if (priv)
    {
        priv->cond          = 1;
        priv->initialized   = true;

        //priv->req           = *req;
        //priv->parent        = parent;
    }

    return priv;
}

void load_system_destroy(void *p)
{
    PrivInfo *thiz = p;
    if (thiz != NULL)
    {
        load_system_stop(thiz);

        pthread_mutex_destroy(&thiz->mutex);

        free(thiz);
    }
}
