/**
 * History:
 * ================================================================
 * 2019-08-15 qing.zou created
 *
 */

#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "load.h"
#include "load_core.h"

typedef struct _PrivInfo
{
    int                     cond;

	pthread_t               core_thread;
    pthread_mutex_t         core_mutex;
	// os_event_t                 *stop_event;

	bool                    initialized;

	// audio_input_callback_t     input_cb;
	// void                       *input_param;
	// pthread_mutex_t            input_mutex;
} PrivInfo;


#include "pbrpc_server.h"
#include "pbrpc_client.h"

static void load_core_test(PrivInfo* thiz)
{
    vmp_config_t* config = global_default_config();
    if (config->server) {
        VMP_LOGD("start server...");
        server(1, NULL);
    } else {
        VMP_LOGD("start client...");
        client();
    }
}

static void* load_core_thread(void* arg)
{
    PrivInfo* thiz = (PrivInfo*)arg;

    load_core_test(thiz);

    while (thiz->cond)
    {
        sleep(5);
    }    

    return NULL;
}

int load_core_start(PrivInfo *thiz)
{
    int ret = 0;
    pthread_mutexattr_t attr;

    if (!thiz)
        return -1;
    
    if (pthread_mutexattr_init(&attr) != 0)
        goto fail;
    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
        goto fail;
    if (pthread_mutex_init(&thiz->core_mutex, &attr) != 0)
        goto fail;

    ret = pthread_create(&thiz->core_thread, NULL, load_core_thread, (void*)thiz);
    if (ret != 0)
        VMP_LOGE("create thread 'load_core_thread' failed");
    
    return 0;

fail:
    load_core_done();
    return -1;
}

int load_core_stop(PrivInfo *thiz)
{
    void *thread_result;

    if (!thiz)
        return -1;

    if (thiz->initialized) {
        thiz->initialized = false;
        pthread_join(thiz->core_thread, &thread_result);
    }

    return 0;
}

void load_core_init(void)
{
    rt_load_t *load = NULL;
    PrivInfo *thiz = calloc(1, sizeof(PrivInfo));

    thiz->cond  = 1;
    thiz->initialized = true;

    load = global_default_load();
    load->core = thiz;

    load_core_start(thiz);
}

void load_core_done(void)
{
    PrivInfo *thiz;
    rt_load_t *load = global_default_load();
    
    thiz = load->core;
    if (thiz != NULL)
    {
        load_core_stop(thiz);

        pthread_mutex_destroy(&thiz->core_mutex);

        free(thiz);
        load->core = NULL;
    }
}

/** business: load **/
void load_init(int argc, char **argv)
{
    rt_load_t *load = NULL;
    void *config = vmp_config_create();
    global_set_config(config);
    vmp_config_init(config, argc, argv);
    
    load = calloc(1, sizeof(rt_load_t));
    global_set_load(load);

    mod_log_init(PROCESS_DAEMON, NULL);

    load_child_start(argc, argv);

    load_core_init();
}

void load_start(void)
{   
    while (1)
    {
        sleep(10);
    }    
}

void load_done(void)
{
    rt_load_t *load;

    load_core_done();

    load = global_default_load();
    if (load) {
        free(load);
        global_set_load(NULL);
    }
}
