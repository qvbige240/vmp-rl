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

#include "workload.pb-c.h"
#include "../pbc/pbrpc-clnt.h"

#define DBUG(x) fprintf (stdout, "%s was called\n", # x)

static int workload_reply_cb(pbrpc_clnt *clnt, ProtobufCBinaryData *msg, int ret)
{
    if (ret)
        return ret;

    PbcWorkloadRsp *rsp = pbc_workload_rsp__unpack(NULL, msg->len, msg->data);

    printf("rsp->id = %d\n", rsp->id);
    printf("rsp->video_cnt = %d\n", rsp->video_cnt);
    printf("rsp->info_mem = %lu\n", rsp->info_mem);

    pbc_workload_rsp__free_unpacked(rsp, NULL);
    return 0;
}

static ProtobufCBinaryData build_workload_args(PbcWorkloadReq *req, int id)
{
    size_t clen;
    unsigned char *cbuf;
    ProtobufCBinaryData pbc_req;

    req->id = id;
    clen = pbc_workload_req__get_packed_size(req);
    cbuf = calloc(1, clen);
    pbc_workload_req__pack(req, cbuf);
    pbc_req.data = cbuf;
    pbc_req.len = clen;

    return pbc_req;
}

int workload_call(pbrpc_clnt *clnt)
{
    int ret;
    PbcWorkloadReq req = PBC_WORKLOAD_REQ__INIT;

    ProtobufCBinaryData msg = build_workload_args(&req, 2);
    ret = pbrpc_clnt_call(clnt, "Loader.workload", &msg, workload_reply_cb);
    if (ret) {
        fprintf(stderr, "RPC call failed\n");
    }

    //free(msg.data);

    return ret;
}
