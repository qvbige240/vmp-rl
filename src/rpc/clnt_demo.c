/**
 * History:
 * ================================================================
 * 2019-10-06 qing.zou created
 *
 */

#include "clnt_demo.h"

#include "pbc/pbrpc-clnt.h"
#include "service/pbc-demo.pb-c.h"

typedef struct _PrivInfo
{
    ClntDemoReq         req;

    void                *parent;
} PrivInfo;

static void clnt_demo_delete(PrivInfo *thiz)
{
    if (thiz)
    {
        free(thiz);
    }
}

static ProtobufCBinaryData demo_request_pack(PbcDemoReq *req)
{
    size_t clen;
    unsigned char *cbuf;
    ProtobufCBinaryData pbc_req;

    clen = pbc_demo_req__get_packed_size(req);
    cbuf = calloc(1, clen);
    pbc_demo_req__pack(req, cbuf);
    pbc_req.data = cbuf;
    pbc_req.len = clen;

    return pbc_req;
}

static int demo_reply_cb(void *ctx, ProtobufCBinaryData *msg, int ret)
{
    PrivInfo *priv = ctx;
    if (ret) goto end;

    PbcDemoRsp *rsp = pbc_demo_rsp__unpack(NULL, msg->len, msg->data);
    if (!rsp) {
        fprintf(stderr, "response null failed, msg len %d\n", msg->len);

        if (priv && priv->req.pfn_callback)
            priv->req.pfn_callback(priv->req.ctx, -1, NULL);

        ret = -1;
        goto end;
    }

    printf("rsp->sid = %d\n", rsp->id);
    printf("rsp->video_cnt = %d\n", rsp->video_cnt);
    printf("rsp->info_mem = %ld\n", rsp->info_mem);
    if (priv && priv->req.pfn_callback)
    {
        ClntDemoRsp info = {0};
        info.id     = rsp->id;
        info.count  = rsp->video_cnt;
        info.memory = rsp->info_mem;
        priv->req.pfn_callback(priv->req.ctx, 0, &info);
    }

    pbc_demo_rsp__free_unpacked(rsp, NULL);

end:
    clnt_demo_delete(priv);
    return ret;
}

int rpc_clnt_demo(vmp_rpclnt_t *thiz, ClntDemoReq *info)
{
    int ret;
    PrivInfo *priv = calloc(1, sizeof(PrivInfo));
    PbcDemoReq req = PBC_DEMO_REQ__INIT;

    priv->req       = *info;
    priv->parent    = thiz;

    req.id          = info->id;
    ProtobufCBinaryData msg = demo_request_pack(&req);

    ret = pbrpc_clnt_call(thiz->clnt, "Loader.demo", &msg, demo_reply_cb, priv);
    if (ret) {
        fprintf(stderr, "RPC call failed\n");
    }

    free(msg.data);

    return ret;
}
