/**
 * History:
 * ================================================================
 * 2019-08-20 qing.zou created
 *
 */

#include "clnt_server_registry.h"

#include "pbc/pbrpc-clnt.h"
#include "service/pbc-registry.pb-c.h"

typedef struct _PrivInfo
{
    ClntServerRegistryReq       req;

    void                        *parent;
} PrivInfo;

static void clnt_registry_delete(PrivInfo *thiz)
{
    if (thiz)
    {
        free(thiz);
    }
}

static ProtobufCBinaryData registry_request_pack(PbcRegistryReq *req)
{
    size_t clen;
    unsigned char *cbuf;
    ProtobufCBinaryData pbc_req;

    clen = pbc_registry_req__get_packed_size(req);
    cbuf = calloc(1, clen);
    pbc_registry_req__pack(req, cbuf);
    pbc_req.data = cbuf;
    pbc_req.len = clen;

    return pbc_req;
}

static int registry_reply_cb(void *ctx, ProtobufCBinaryData *msg, int ret)
{
    PrivInfo *priv = ctx;
    if (ret) goto end;

    PbcRegistryRsp *rsp = pbc_registry_rsp__unpack(NULL, msg->len, msg->data);
    if (!rsp)
    {
        fprintf(stderr, "response null failed, msg len %d\n", msg->len);

        if (priv && priv->req.pfn_callback)
            priv->req.pfn_callback(priv->req.ctx, -1, NULL);

        ret = -1;
        goto end;
    }

    printf("rsp->sid = %d\n", rsp->sid);
    printf("rsp->name = %s\n", rsp->name);
    printf("rsp->index = %d\n", rsp->index);
    printf("rsp->num = %d\n", rsp->num);
    if (priv && priv->req.pfn_callback)
    {
        ClntServerRegistryRsp info = {0};
        info.id     = rsp->sid;
        info.name   = rsp->name;
        info.index  = rsp->index;
        if (rsp->has_num)
            info.num = rsp->num;
        priv->req.pfn_callback(priv->req.ctx, 0, &info);
    }

    pbc_registry_rsp__free_unpacked(rsp, NULL);
end:
    clnt_registry_delete(priv);
    return ret;
}

int rpc_clnt_registry(vmp_rpclnt_t *thiz, ClntServerRegistryReq *info)
{
    int ret;
    PrivInfo *priv = calloc(1, sizeof(PrivInfo));
    PbcRegistryReq req = PBC_REGISTRY_REQ__INIT;

    priv->req         = *info;
    priv->parent      = thiz;

    req.sid           = info->id;
    req.name          = info->name;
    req.system        = info->system;
    req.location      = info->location;
    req.processor     = info->processor;
    req.bandwidth     = info->bandwidth;
    req.memory        = info->memory;
    req.ip            = info->ip;
    req.port          = info->port;
    ProtobufCBinaryData msg = registry_request_pack(&req);

    ret = pbrpc_clnt_call(thiz->clnt, "Loader.registry", &msg, registry_reply_cb, priv);
    if (ret)
    {
        fprintf(stderr, "RPC call failed\n");
    }

    free(msg.data);

    return ret;
}
