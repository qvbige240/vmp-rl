/**
 * History:
 * ================================================================
 * 2019-08-20 qing.zou created
 *
 */

#include "rpc_server_info.h"

#include "pbc/pbrpc-clnt.h"
#include "service/pbc-registry.pb-c.h"


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

static int registry_reply_cb(pbrpc_clnt *clnt, ProtobufCBinaryData *msg, int ret)
{
    if (ret) return ret;

    PbcRegistryRsp *rsp = pbc_registry_rsp__unpack(NULL, msg->len, msg->data);

    printf("rsp->sid = %d\n", rsp->sid);
    printf("rsp->name = %s\n", rsp->name);
    printf("rsp->ip = %s\n", rsp->ip);
    printf("rsp->num = %d\n", rsp->num);

    pbc_registry_rsp__free_unpacked(rsp, NULL);
    return 0;
}

int rpc_call_registry(void* clnt, RpcServerInfoReq *info)
{
    int ret;
    PbcRegistryReq req = PBC_REGISTRY_REQ__INIT;

    req.sid     = info->id;
    req.name    = info->name;
    req.system  = info->system;
    req.location    = info->location;
    req.bandwidth   = info->bandwidth;
    req.ip          = info->ip;
    req.port        = info->port;
    ProtobufCBinaryData msg = registry_request_pack(&req);
    ret = pbrpc_clnt_call(clnt, "Loader.registry", &msg, registry_reply_cb);
    if (ret) {
        fprintf(stderr, "RPC call failed\n");
    }

    free(msg.data);

    return ret;
}
