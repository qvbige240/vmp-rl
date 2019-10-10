/**
 * History:
 * ================================================================
 * 2019-10-08 qing.zou created
 *
 */

#include "clnt_node_state.h"

#include "pbc/pbrpc-clnt.h"
#include "service/pbc-nstatus.pb-c.h"

typedef struct _PrivInfo
{
    ClntNodeStateReq        req;

    void                    *parent;
} PrivInfo;

static void clnt_node_state_delete(PrivInfo *thiz)
{
    if (thiz)
    {
        free(thiz);
    }
}

static ProtobufCBinaryData node_state_request_pack(PbcNodeStateReq *req)
{
    size_t clen;
    unsigned char *cbuf;
    ProtobufCBinaryData pbc_req;

    clen = pbc_node_state_req__get_packed_size(req);
    cbuf = calloc(1, clen);
    pbc_node_state_req__pack(req, cbuf);
    pbc_req.data = cbuf;
    pbc_req.len = clen;

    return pbc_req;
}

static int node_state_reply_cb(void *ctx, ProtobufCBinaryData *msg, int ret)
{
    PrivInfo *priv = ctx;
    if (ret) goto end;

    PbcNodeStateRsp *rsp = pbc_node_state_rsp__unpack(NULL, msg->len, msg->data);
    if (!rsp)
    {
        fprintf(stderr, "response null failed, msg len %d\n", msg->len);

        if (priv && priv->req.pfn_callback)
            priv->req.pfn_callback(priv->req.ctx, -1, NULL);

        ret = -1;
        goto end;
    }

    printf("rsp->id = %d\n", rsp->id);
    printf("rsp->count = %d\n", rsp->count);
    printf("rsp->uplink = %ld\n", rsp->uplink);
    if (priv && priv->req.pfn_callback)
    {
        ClntNodeStateRsp info = {0};
        info.id         = rsp->id;
        info.index      = rsp->index;
        info.name       = rsp->name;
        info.count      = rsp->count;
        info.uplink     = rsp->uplink;
        info.downlink   = rsp->downlink;
        info.memory     = rsp->memory;
        info.cpu        = rsp->cpu;
        priv->req.pfn_callback(priv->req.ctx, 0, &info);
    }

    pbc_node_state_rsp__free_unpacked(rsp, NULL);

end:
    clnt_node_state_delete(priv);
    return ret;
}

int rpc_clnt_node_state(vmp_rpclnt_t *thiz, ClntNodeStateReq *info)
{
    int ret;
    PrivInfo *priv = calloc(1, sizeof(PrivInfo));
    PbcNodeStateReq req = PBC_NODE_STATE_REQ__INIT;

    priv->req       = *info;
    priv->parent    = thiz;

    req.id          = info->id;
    ProtobufCBinaryData msg = node_state_request_pack(&req);

    ret = pbrpc_clnt_call(thiz->clnt, "Loader.node_state", &msg, node_state_reply_cb, priv);
    if (ret)
    {
        fprintf(stderr, "RPC call failed\n");
    }

    free(msg.data);

    return ret;
}
