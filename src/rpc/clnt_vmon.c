/**
 * History:
 * ================================================================
 * 2019-09-02 qing.zou created
 *
 */

#include "clnt_vmon.h"

#include "pbc/pbrpc-clnt.h"
#include "service/pbc-vmon.pb-c.h"

typedef struct _PrivInfo
{
    ClntVmonReq         req;

    void                *parent;
} PrivInfo;

static void clnt_vmon_delete(PrivInfo *thiz)
{
    if (thiz)
    {
        free(thiz);
    }
}

static ProtobufCBinaryData registry_request_pack(PbcVmonReq *req)
{
    size_t clen;
    unsigned char *cbuf;
    ProtobufCBinaryData pbc_req;

    clen = pbc_vmon_req__get_packed_size(req);
    cbuf = calloc(1, clen);
    pbc_vmon_req__pack(req, cbuf);
    pbc_req.data = cbuf;
    pbc_req.len = clen;

    return pbc_req;
}

static int vmon_reply_cb(void *ctx, ProtobufCBinaryData *msg, int ret)
{
    PrivInfo *priv = ctx;
    if (ret) goto end;

    PbcVmonRsp *rsp = pbc_vmon_rsp__unpack(NULL, msg->len, msg->data);
    if (!rsp) {
        fprintf(stderr, "response null failed, msg len %d\n", msg->len);

        if (priv && priv->req.pfn_callback)
            priv->req.pfn_callback(priv->req.ctx, -1, NULL);

        ret = -1;
        goto end;
    }

    printf("rsp->sid = %d\n", rsp->id);
    printf("rsp->count = %d\n", rsp->count);
    if (priv && priv->req.pfn_callback)
    {
        ClntVmonRsp info = {0};
        info.id     = rsp->id;
        info.count   = rsp->count;
        priv->req.pfn_callback(priv->req.ctx, 0, &info);
    }

    pbc_vmon_rsp__free_unpacked(rsp, NULL);

end:
    clnt_vmon_delete(priv);
    return ret;
}

int rpc_clnt_vmon(vmp_rpclnt_t *thiz, ClntVmonReq *info)
{
    int ret;
    PrivInfo *priv = calloc(1, sizeof(PrivInfo));
    PbcVmonReq req = PBC_VMON_REQ__INIT;

    priv->req       = *info;
    priv->parent    = thiz;

    req.id          = info->id;
    ProtobufCBinaryData msg = registry_request_pack(&req);

    ret = pbrpc_clnt_call(thiz->clnt, "Loader.vmon", &msg, vmon_reply_cb, priv);
    if (ret) {
        fprintf(stderr, "RPC call failed\n");
    }

    free(msg.data);

    return ret;
}
