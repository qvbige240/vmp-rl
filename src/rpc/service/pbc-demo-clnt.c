/**
 * History:
 * ================================================================
 * 2019-08-07 qing.zou created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "pbc-demo.pb-c.h"
#include "../pbc/pbrpc-clnt.h"

#define DBUG(x) fprintf (stdout, "%s was called\n", # x)

static int demo_reply_cb(void *ctx, ProtobufCBinaryData *msg, int ret)
{
    if (ret)
        return ret;

    PbcDemoRsp *rsp = pbc_demo_rsp__unpack(NULL, msg->len, msg->data);

    printf("rsp->id = %d\n", rsp->id);
    printf("rsp->video_cnt = %d\n", rsp->video_cnt);
    printf("rsp->info_mem = %lu\n", rsp->info_mem);

    pbc_demo_rsp__free_unpacked(rsp, NULL);
    return 0;
}

static ProtobufCBinaryData build_demo_args(PbcDemoReq *req, int id)
{
    size_t clen;
    unsigned char *cbuf;
    ProtobufCBinaryData pbc_req;

    req->id = id;
    clen = pbc_demo_req__get_packed_size(req);
    cbuf = calloc(1, clen);
    pbc_demo_req__pack(req, cbuf);
    pbc_req.data = cbuf;
    pbc_req.len = clen;

    return pbc_req;
}

int demo_call(pbrpc_clnt *clnt)
{
    int ret;
    PbcDemoReq req = PBC_DEMO_REQ__INIT;

    ProtobufCBinaryData msg = build_demo_args(&req, 2);
    ret = pbrpc_clnt_call(clnt, "Loader.demo", &msg, demo_reply_cb, NULL);
    if (ret) {
        fprintf(stderr, "RPC call failed\n");
    }

    //free(msg.data);

    return ret;
}
