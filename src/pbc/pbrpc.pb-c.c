/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: pbrpc.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "pbrpc.pb-c.h"
void   pbc_rpc_request__init
                     (PbcRpcRequest         *message)
{
  static const PbcRpcRequest init_value = PBC_RPC_REQUEST__INIT;
  *message = init_value;
}
size_t pbc_rpc_request__get_packed_size
                     (const PbcRpcRequest *message)
{
  assert(message->base.descriptor == &pbc_rpc_request__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t pbc_rpc_request__pack
                     (const PbcRpcRequest *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &pbc_rpc_request__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t pbc_rpc_request__pack_to_buffer
                     (const PbcRpcRequest *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &pbc_rpc_request__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
PbcRpcRequest *
       pbc_rpc_request__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (PbcRpcRequest *)
     protobuf_c_message_unpack (&pbc_rpc_request__descriptor,
                                allocator, len, data);
}
void   pbc_rpc_request__free_unpacked
                     (PbcRpcRequest *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &pbc_rpc_request__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   pbc_rpc_response__init
                     (PbcRpcResponse         *message)
{
  static const PbcRpcResponse init_value = PBC_RPC_RESPONSE__INIT;
  *message = init_value;
}
size_t pbc_rpc_response__get_packed_size
                     (const PbcRpcResponse *message)
{
  assert(message->base.descriptor == &pbc_rpc_response__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t pbc_rpc_response__pack
                     (const PbcRpcResponse *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &pbc_rpc_response__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t pbc_rpc_response__pack_to_buffer
                     (const PbcRpcResponse *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &pbc_rpc_response__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
PbcRpcResponse *
       pbc_rpc_response__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (PbcRpcResponse *)
     protobuf_c_message_unpack (&pbc_rpc_response__descriptor,
                                allocator, len, data);
}
void   pbc_rpc_response__free_unpacked
                     (PbcRpcResponse *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &pbc_rpc_response__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor pbc_rpc_request__field_descriptors[3] =
{
  {
    "Id",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT64,
    0,   /* quantifier_offset */
    offsetof(PbcRpcRequest, id),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "Method",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(PbcRpcRequest, method),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "Params",
    3,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_BYTES,
    offsetof(PbcRpcRequest, has_params),
    offsetof(PbcRpcRequest, params),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned pbc_rpc_request__field_indices_by_name[] = {
  0,   /* field[0] = Id */
  1,   /* field[1] = Method */
  2,   /* field[2] = Params */
};
static const ProtobufCIntRange pbc_rpc_request__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor pbc_rpc_request__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "pbc.RpcRequest",
  "RpcRequest",
  "PbcRpcRequest",
  "pbc",
  sizeof(PbcRpcRequest),
  3,
  pbc_rpc_request__field_descriptors,
  pbc_rpc_request__field_indices_by_name,
  1,  pbc_rpc_request__number_ranges,
  (ProtobufCMessageInit) pbc_rpc_request__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor pbc_rpc_response__field_descriptors[3] =
{
  {
    "Id",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT64,
    0,   /* quantifier_offset */
    offsetof(PbcRpcResponse, id),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "Result",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_BYTES,
    0,   /* quantifier_offset */
    offsetof(PbcRpcResponse, result),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "Error",
    3,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(PbcRpcResponse, error),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned pbc_rpc_response__field_indices_by_name[] = {
  2,   /* field[2] = Error */
  0,   /* field[0] = Id */
  1,   /* field[1] = Result */
};
static const ProtobufCIntRange pbc_rpc_response__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor pbc_rpc_response__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "pbc.RpcResponse",
  "RpcResponse",
  "PbcRpcResponse",
  "pbc",
  sizeof(PbcRpcResponse),
  3,
  pbc_rpc_response__field_descriptors,
  pbc_rpc_response__field_indices_by_name,
  1,  pbc_rpc_response__number_ranges,
  (ProtobufCMessageInit) pbc_rpc_response__init,
  NULL,NULL,NULL    /* reserved[123] */
};
