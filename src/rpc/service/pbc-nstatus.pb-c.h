/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: pbc-nstatus.proto */

#ifndef PROTOBUF_C_pbc_2dnstatus_2eproto__INCLUDED
#define PROTOBUF_C_pbc_2dnstatus_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1000000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003002 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _PbcNodeStateReq PbcNodeStateReq;
typedef struct _PbcNodeStateRsp PbcNodeStateRsp;


/* --- enums --- */


/* --- messages --- */

struct  _PbcNodeStateReq
{
  ProtobufCMessage base;
  uint32_t id;
};
#define PBC_NODE_STATE_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&pbc_node_state_req__descriptor) \
    , 0 }


struct  _PbcNodeStateRsp
{
  ProtobufCMessage base;
  uint32_t id;
  uint32_t index;
  char *name;
  uint32_t count;
  uint64_t uplink;
  uint64_t downlink;
  uint64_t memory;
  double cpu;
};
#define PBC_NODE_STATE_RSP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&pbc_node_state_rsp__descriptor) \
    , 0, 0, NULL, 0, 0, 0, 0, 0 }


/* PbcNodeStateReq methods */
void   pbc_node_state_req__init
                     (PbcNodeStateReq         *message);
size_t pbc_node_state_req__get_packed_size
                     (const PbcNodeStateReq   *message);
size_t pbc_node_state_req__pack
                     (const PbcNodeStateReq   *message,
                      uint8_t             *out);
size_t pbc_node_state_req__pack_to_buffer
                     (const PbcNodeStateReq   *message,
                      ProtobufCBuffer     *buffer);
PbcNodeStateReq *
       pbc_node_state_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   pbc_node_state_req__free_unpacked
                     (PbcNodeStateReq *message,
                      ProtobufCAllocator *allocator);
/* PbcNodeStateRsp methods */
void   pbc_node_state_rsp__init
                     (PbcNodeStateRsp         *message);
size_t pbc_node_state_rsp__get_packed_size
                     (const PbcNodeStateRsp   *message);
size_t pbc_node_state_rsp__pack
                     (const PbcNodeStateRsp   *message,
                      uint8_t             *out);
size_t pbc_node_state_rsp__pack_to_buffer
                     (const PbcNodeStateRsp   *message,
                      ProtobufCBuffer     *buffer);
PbcNodeStateRsp *
       pbc_node_state_rsp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   pbc_node_state_rsp__free_unpacked
                     (PbcNodeStateRsp *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*PbcNodeStateReq_Closure)
                 (const PbcNodeStateReq *message,
                  void *closure_data);
typedef void (*PbcNodeStateRsp_Closure)
                 (const PbcNodeStateRsp *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor pbc_node_state_req__descriptor;
extern const ProtobufCMessageDescriptor pbc_node_state_rsp__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_pbc_2dnstatus_2eproto__INCLUDED */
