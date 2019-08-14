/**
 * History:
 * ================================================================
 * 2019-08-07 qing.zou created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <errno.h>
#include <inttypes.h>
#include <arpa/inet.h>

#include <event2/buffer.h>
#include <event2/listener.h>

#include "list.h"
#include "pbrpc-svc.h"

#define DECL_CALLCOUNT(x) static int x = 0
#define UP_CALLCOUNT(x) (x)++
#define PRINT_CALLCOUNT(dom, x) fprintf (stdout, "%s: call no. %d\n", (dom), (x));

static int rpc_write_reply(pbrpc_svc *svc, PbcRpcResponse *rsphdr, unsigned char **buf);
static int rpc_invoke_call(pbrpc_svc *svc, PbcRpcRequest *reqhdr, PbcRpcResponse *rsphdr);
static PbcRpcRequest* rpc_read_req(pbrpc_svc *svc, const unsigned char *msg, uint64_t *bytes_read);

void generic_event_cb(struct bufferevent *bev, short events, void *ctx)
{
    if (events & BEV_EVENT_ERROR)
        perror("Error from bufferevent");
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        bufferevent_free(bev);
    }
    printf("event_cb events(0x%02x)\n", events);
}

void generic_read_cb(struct bufferevent *bev, void *ctx)
{
    struct pbrpc_xdr *xdr = ctx;
    unsigned char *buf = NULL;
    unsigned char *msg = NULL;
    struct evbuffer *in = NULL;
    size_t len = 0;
    size_t read = 0;

    in = bufferevent_get_input(bev);
    len = evbuffer_get_length(in);

    buf = calloc(1, len);
    if (!buf)
    {
        fprintf(stderr, "Failed to allocate memory\n");
        return;
    }

    read = bufferevent_read(bev, buf, len);
    if (read < 0)
    {
        perror("bufferevent_read failed");
        goto out;
    }

    uint64_t bytes_read = 0;
    for (msg = buf; read > 0; read -= bytes_read, msg += bytes_read)
        bytes_read = xdr->proc(xdr, bev, msg);

out:
    free(buf);
}

static int process_request(void *handle, struct bufferevent *bev, unsigned char *msg)
{
    uint64_t bytes_read = 0;
    struct pbrpc_xdr *xdr = handle;
    pbrpc_svc *svc = NULL;
    int ret = -1;
    unsigned char *outbuf = NULL;

    svc = list_entry(xdr, pbrpc_svc, xdrs);

    PbcRpcRequest *reqhdr = rpc_read_req(svc, msg, &bytes_read);
    PbcRpcResponse rsphdr = PBC_RPC_RESPONSE__INIT;
    ret = rpc_invoke_call(svc, reqhdr, &rsphdr);
    if (ret) {
        fprintf(stderr, "ret = %d: rpc_invoke_call failed\n", ret);
    }

    outbuf = NULL;
    ret = rpc_write_reply(svc, &rsphdr, &outbuf);
    if (ret <= 0) {
        fprintf(stderr, "ret = %d: rpc_write_reply failed\n", ret);
    }

    bufferevent_write(bev, outbuf, ret);

    pbc_rpc_request__free_unpacked(reqhdr, NULL);
    free(outbuf);

    return bytes_read;
}

enum bufferevent_filter_result filter_pbrpc_messages(struct evbuffer *src,
                                     struct evbuffer *dst, ev_ssize_t dst_limit,
                                     enum bufferevent_flush_mode mode, void *ctx)
{
    uint64_t message_len = 0;
    size_t buf_len = 0;
    size_t expected_len = 0;
    size_t tmp = 0;
    unsigned char *lenbuf = NULL;
    //struct bufferevent *bev = ctx;

    /**
     * Read the message len evbuffer_pullup doesn't drain the buffer. It
     * just makes sure that given number of bytes are contiguous, which
     * allows us to easily copy out stuff.
     */
    lenbuf = evbuffer_pullup(src, sizeof(message_len));
    if (!lenbuf)
        return BEV_ERROR;

    memcpy(&message_len, lenbuf, sizeof(message_len));

    message_len = be64toh(message_len);

    expected_len = sizeof(message_len) + message_len;

    buf_len = evbuffer_get_length(src);
    if (buf_len < expected_len)
        return BEV_NEED_MORE;

    tmp = evbuffer_remove_buffer(src, dst, expected_len);
    if (tmp != expected_len)
        return BEV_ERROR;

    return BEV_OK;
}

static void accept_conn_cb(struct evconnlistener *listener,
                evutil_socket_t fd, struct sockaddr *address, 
                int socklen, void *ctx)
{
    pbrpc_svc *svc = ctx;

    /* We got a new connection! Set up a bufferevent for it. */
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    printf("accept: fd(%d)\n", fd);
    struct bufferevent *filtered_bev = bufferevent_filter_new(
        bev, filter_pbrpc_messages, NULL, BEV_OPT_CLOSE_ON_FREE, NULL, bev);

    bufferevent_setcb(filtered_bev, generic_read_cb, NULL, generic_event_cb, &svc->xdrs);
    bufferevent_enable(filtered_bev, EV_READ | EV_WRITE);
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx)
{
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got an error %d (%s) on the listener. "
                    "Shutting down.\n", err, evutil_socket_error_to_string(err));
    fflush(stderr);
    pbrpc_svc_destroy((pbrpc_svc *)ctx);
}

int pbrpc_svc_register_methods(pbrpc_svc *svc, pbrpc_svc_callout *methods)
{
    svc->methods = methods;
    return 0;
}

int pbrpc_svc_destroy(pbrpc_svc *svc)
{
    struct event_base *base = evconnlistener_get_base(svc->listener);

    event_base_loopexit(base, NULL);
    event_base_free(base);
    free(svc);
    return 0;
}

pbrpc_svc* pbrpc_svc_new(const char *name, int16_t port)
{
    struct event_base *base = NULL;
    struct sockaddr_in sin;
    struct evconnlistener *listener = NULL;

    pbrpc_svc *new = calloc (1, sizeof (*new));
    if (!new)
        return NULL;

    base = event_base_new();
    if (!base)
        goto err;

    /* Clear the sockaddr before using it, in case there are extra
     * platform-specific fields that can mess us up. */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    /* Listen on 0.0.0.0 */
    //FIXME: @name is ignored. Use getaddrinfo(3) to fill in_addr appropriately.
    sin.sin_addr.s_addr = htonl(0);
    /* Listen on the given port. */
    sin.sin_port = htons(port);

    listener = evconnlistener_new_bind(base, accept_conn_cb, new,
                LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1,
                (struct sockaddr*)&sin, sizeof(sin));
    if (!listener) {
        perror("Couldn't create listener");
        goto err;
    }
    evconnlistener_set_error_cb(listener, accept_error_cb);
    new->listener = listener;
    new->xdrs.proc = process_request;

    return new;
err:
    free (new);
    return NULL;
}

int pbrpc_svc_run(pbrpc_svc *svc)
{
    struct evconnlistener *listener = svc->listener;
    struct event_base *base = evconnlistener_get_base(listener);

    return event_base_dispatch(base);
}

/*
 * RPC Format
 * -----------------------------------------------------
 * | Length: uint64                                    |
 * -----------------------------------------------------
 * |                                                   |
 * | Header:  Id uint64, Method int32, Params []bytes |
 * |                                                   |
 * -----------------------------------------------------
 * |                                                   |
 * | Body: Method dependent representation             |
 * |                                                   |
 * -----------------------------------------------------
 *
 **/

/*
 * pseudo code:
 *
 * read buffer from network
 * reqhdr = rpc_read_req (svc, msg, len);
 * rsphdr = RSP_HEADER__INIT;
 * ret = rpc_invoke_call (svc, reqhdr, &rsphdr)
 * if (ret)
 *   goto out;
 * char *buf = NULL;
 * ret = rpc_write_reply (svc, &rsphdr, &buf);
 * if (ret != -1)
 *   goto out;
 *
 * send buf over n/w
 *
 * free(buf);
 * free(rsp->result->data);
 *
 *
 * ...
 * clean up protobuf buffers on finish
 *
 * */

/* returns the no. of bytes written to @buf
 * @buf is allocated by this function.
 * */

/* returns the no. of bytes written to @buf
 * @buf is allocated by this function.
 * */
static int rpc_write_reply(pbrpc_svc *svc, PbcRpcResponse *rsphdr, unsigned char **buf)
{
    uint64_t be_len = 0;
    if (!buf)
        return -1;

    size_t rsplen = pbc_rpc_response__get_packed_size(rsphdr);
    *buf = calloc(1, rsplen + sizeof(uint64_t));
    if (!*buf)
        return -1;

    be_len = htobe64(rsplen);
    pbc_rpc_response__pack(rsphdr, *buf + sizeof(uint64_t));
    memcpy(*buf, &be_len, sizeof(be_len));
    return rsplen + sizeof(uint64_t);
}

static int rpc_invoke_call(pbrpc_svc *svc, PbcRpcRequest *reqhdr, PbcRpcResponse *rsphdr)
{
    int ret;

    if (!reqhdr->has_params)
    {
        fprintf(stderr, "no params passed\n");
        return -1;
    }
    rsphdr->id = reqhdr->id;

    pbrpc_svc_callout *method;
    for (method = svc->methods; method; method++)
        if (!strcmp(method->name, reqhdr->method))
            break;

    if (!method)
        return -1;

    ret = method->dispatch(&reqhdr->params, &rsphdr->result);

    return ret;
}

static PbcRpcRequest* rpc_read_req(pbrpc_svc *svc, const unsigned char *msg, uint64_t *bytes_read)
{
    uint64_t proto_len = 0;

    memcpy(&proto_len, msg, sizeof(uint64_t));
    proto_len = be64toh(proto_len);
    *bytes_read = proto_len + sizeof(proto_len);
    return pbc_rpc_request__unpack(NULL, proto_len, msg + sizeof(proto_len));
}
