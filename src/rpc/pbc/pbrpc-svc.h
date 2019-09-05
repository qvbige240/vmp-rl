/**
 * History:
 * ================================================================
 * 2019-08-07 qing.zou created
 *
 */

#ifndef PBRPC_SVC_H
#define PBRPC_SVC_H

#include <pthread.h>
#include <event2/event.h>
#include <event2/bufferevent.h>

#include "pbrpc.pb-c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Rpcsvc method type
 */
typedef int (*pbrpc_svc_dispatch)(void *handler, ProtobufCBinaryData *req, ProtobufCBinaryData *rsp);

/**
 * Rpcsvc method entry
 */
typedef struct pbrpc_svc_callout
{
    struct pbrpc_svc_callout    *sc_next;
    uint32_t                    sc_prog;
    uint32_t                    sc_vers;
    char                        *sc_name;
    void                        *sc_args;
    pbrpc_svc_dispatch          dispatch;
} pbrpc_svc_callout;

/**
 * External data representation serialization routines.
 */
typedef int (*pbrpc_xdrproc)(void *handle, struct bufferevent *bev, unsigned char *buf);

struct pbrpc_xdr
{
    pbrpc_xdrproc proc;
};

typedef struct pbrpc_svc
{
    struct evconnlistener   *listener;
    void                    *ctx;
    /**
     * Callbacks for accepted connection
     * */
    struct pbrpc_xdr        xdrs;
    bufferevent_data_cb     reader;
    bufferevent_event_cb    notifier;

    pbrpc_svc_callout       *methods;

    pbrpc_svc_callout       *svc_head;
    pthread_rwlock_t	    svc_lock;
} pbrpc_svc;

/**
 * Creates a new object en pbrpc based service listening at hostname on port
 *
 * @param hostname - a string representing the hostname or IP address of the
 *                 node
 *
 * @param port - the port on which the service needs to listening
 *
 * @return  - a pointer to a newly allocated pbrpc_svc object, or NULL if an error
 *            occurred
 *
 * */

pbrpc_svc* pbrpc_svc_new(const char *hostname, int16_t port);


/**
 * Registers RPC methods with a pbrpc_svc object
 *
 * @param svc - the service object
 * @param methods - a list of functions that are exported via @pbrpc_svc
 * @return - 0 on success, -1 on failure
 *
 * */
int pbrpc_svc_register_methods(pbrpc_svc *svc, pbrpc_svc_callout *methods);

int pbrpc_svc_register(pbrpc_svc *svc, uint32_t prog, uint32_t vers, char *name,
                       pbrpc_svc_dispatch dispatch, void *args);
void pbrpc_svc_unregister(pbrpc_svc *svc, uint32_t prog, uint32_t vers, char *name);

/**
 * Starts the event_base_loop (ref: libevent2) to listen for requests on @svc.
 * Blocks the current thread until pbrpc_svc_destroy is called
 *
 * @param svc - the service object
 * @return - 0 on success, -1 on failure
 *
 * */
int pbrpc_svc_run(pbrpc_svc *svc);

/**
 * Stops the @svc's event_base_loop and frees @svc.
 * @return - 0 on success, -1 on failure
 *
 * */
int pbrpc_svc_destroy(pbrpc_svc *svc);

/* Rpcproto Method constants */
enum method_type
{
    CALCULATE = 1
};

typedef int (*rpc_handler_func)(ProtobufCBinaryData *req, ProtobufCBinaryData *reply);

enum bufferevent_filter_result filter_pbrpc_messages(struct evbuffer *src,
                      struct evbuffer *dst, ev_ssize_t dst_limit,
                      enum bufferevent_flush_mode mode, void *ctx);
void generic_event_cb(struct bufferevent *bev, short events, void *ctx);

void generic_read_cb(struct bufferevent *bev, void *ctx);

#ifdef __cplusplus
}
#endif

#endif //PBRPC_SVC_H
