// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: RpcProtocol.proto

#ifndef PROTOBUF_RpcProtocol_2eproto__INCLUDED
#define PROTOBUF_RpcProtocol_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_util.h>
// @@protoc_insertion_point(includes)

namespace Proto {
namespace Protocol {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_RpcProtocol_2eproto();
void protobuf_AssignDesc_RpcProtocol_2eproto();
void protobuf_ShutdownFile_RpcProtocol_2eproto();

class Header;

enum PacketType {
  NONE = 0,
  REQUEST = 1,
  RESPONSE = 2,
  CANCEL = 3,
  LOGIN = 4,
  LOGOUT = 5,
  COMMAND = 6,
  SERVER = 7,
  PacketType_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  PacketType_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool PacketType_IsValid(int value);
const PacketType PacketType_MIN = NONE;
const PacketType PacketType_MAX = SERVER;
const int PacketType_ARRAYSIZE = PacketType_MAX + 1;

enum BattlePacketType {
  POSITION = 0,
  HIT = 1,
  HEAL = 2,
  JOIN_GAME = 3,
  LEAVE_GAME = 4,
  BattlePacketType_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  BattlePacketType_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool BattlePacketType_IsValid(int value);
const BattlePacketType BattlePacketType_MIN = POSITION;
const BattlePacketType BattlePacketType_MAX = LEAVE_GAME;
const int BattlePacketType_ARRAYSIZE = BattlePacketType_MAX + 1;

enum RpcType {
  RPC_SIG_NONE = 0,
  RPC_SIG_USER_LOGIN = 1,
  RPC_SIG_PLAYER_LOGIN = 2,
  RPC_SIG_CLIENT_PROXY = 3,
  RPC_SIG_CHANNEL_INNER_PROXY = 4,
  RPC_SIG_CHAT_INNER_PROXY = 5,
  RPC_SIG_COUNT = 6,
  RpcType_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  RpcType_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool RpcType_IsValid(int value);
const RpcType RpcType_MIN = RPC_SIG_NONE;
const RpcType RpcType_MAX = RPC_SIG_COUNT;
const int RpcType_ARRAYSIZE = RpcType_MAX + 1;

// ===================================================================

class Header : public ::google::protobuf::MessageLite /* @@protoc_insertion_point(class_definition:Proto.Protocol.Header) */ {
 public:
  Header();
  virtual ~Header();

  Header(const Header& from);

  inline Header& operator=(const Header& from) {
    CopyFrom(from);
    return *this;
  }

  static const Header& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const Header* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(Header* other);

  // implements Message ----------------------------------------------

  inline Header* New() const { return New(NULL); }

  Header* New(::google::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);
  void CopyFrom(const Header& from);
  void MergeFrom(const Header& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  void DiscardUnknownFields();
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(Header* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _arena_ptr_;
  }
  inline ::google::protobuf::Arena* MaybeArenaPtr() const {
    return _arena_ptr_;
  }
  public:

  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional .Proto.Protocol.PacketType type = 1;
  void clear_type();
  static const int kTypeFieldNumber = 1;
  ::Proto::Protocol::PacketType type() const;
  void set_type(::Proto::Protocol::PacketType value);

  // optional string methodName = 2;
  void clear_methodname();
  static const int kMethodNameFieldNumber = 2;
  const ::std::string& methodname() const;
  void set_methodname(const ::std::string& value);
  void set_methodname(const char* value);
  void set_methodname(const char* value, size_t size);
  ::std::string* mutable_methodname();
  ::std::string* release_methodname();
  void set_allocated_methodname(::std::string* methodname);

  // optional sint64 uid = 4;
  void clear_uid();
  static const int kUidFieldNumber = 4;
  ::google::protobuf::int64 uid() const;
  void set_uid(::google::protobuf::int64 value);

  // optional sint64 pid = 5;
  void clear_pid();
  static const int kPidFieldNumber = 5;
  ::google::protobuf::int64 pid() const;
  void set_pid(::google::protobuf::int64 value);

  // optional sint32 code = 6;
  void clear_code();
  static const int kCodeFieldNumber = 6;
  ::google::protobuf::int32 code() const;
  void set_code(::google::protobuf::int32 value);

  // optional uint32 rpcId = 7;
  void clear_rpcid();
  static const int kRpcIdFieldNumber = 7;
  ::google::protobuf::uint32 rpcid() const;
  void set_rpcid(::google::protobuf::uint32 value);

  // optional uint32 packetId = 8;
  void clear_packetid();
  static const int kPacketIdFieldNumber = 8;
  ::google::protobuf::uint32 packetid() const;
  void set_packetid(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:Proto.Protocol.Header)
 private:

  ::google::protobuf::internal::ArenaStringPtr _unknown_fields_;
  ::google::protobuf::Arena* _arena_ptr_;

  bool _is_default_instance_;
  ::google::protobuf::internal::ArenaStringPtr methodname_;
  ::google::protobuf::int64 uid_;
  int type_;
  ::google::protobuf::int32 code_;
  ::google::protobuf::int64 pid_;
  ::google::protobuf::uint32 rpcid_;
  ::google::protobuf::uint32 packetid_;
  mutable int _cached_size_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_RpcProtocol_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_RpcProtocol_2eproto();
  #endif
  friend void protobuf_AssignDesc_RpcProtocol_2eproto();
  friend void protobuf_ShutdownFile_RpcProtocol_2eproto();

  void InitAsDefaultInstance();
  static Header* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// Header

// optional .Proto.Protocol.PacketType type = 1;
inline void Header::clear_type() {
  type_ = 0;
}
inline ::Proto::Protocol::PacketType Header::type() const {
  // @@protoc_insertion_point(field_get:Proto.Protocol.Header.type)
  return static_cast< ::Proto::Protocol::PacketType >(type_);
}
inline void Header::set_type(::Proto::Protocol::PacketType value) {
  
  type_ = value;
  // @@protoc_insertion_point(field_set:Proto.Protocol.Header.type)
}

// optional string methodName = 2;
inline void Header::clear_methodname() {
  methodname_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& Header::methodname() const {
  // @@protoc_insertion_point(field_get:Proto.Protocol.Header.methodName)
  return methodname_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Header::set_methodname(const ::std::string& value) {
  
  methodname_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:Proto.Protocol.Header.methodName)
}
inline void Header::set_methodname(const char* value) {
  
  methodname_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Proto.Protocol.Header.methodName)
}
inline void Header::set_methodname(const char* value, size_t size) {
  
  methodname_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Proto.Protocol.Header.methodName)
}
inline ::std::string* Header::mutable_methodname() {
  
  // @@protoc_insertion_point(field_mutable:Proto.Protocol.Header.methodName)
  return methodname_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Header::release_methodname() {
  // @@protoc_insertion_point(field_release:Proto.Protocol.Header.methodName)
  
  return methodname_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Header::set_allocated_methodname(::std::string* methodname) {
  if (methodname != NULL) {
    
  } else {
    
  }
  methodname_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), methodname);
  // @@protoc_insertion_point(field_set_allocated:Proto.Protocol.Header.methodName)
}

// optional sint64 uid = 4;
inline void Header::clear_uid() {
  uid_ = GOOGLE_LONGLONG(0);
}
inline ::google::protobuf::int64 Header::uid() const {
  // @@protoc_insertion_point(field_get:Proto.Protocol.Header.uid)
  return uid_;
}
inline void Header::set_uid(::google::protobuf::int64 value) {
  
  uid_ = value;
  // @@protoc_insertion_point(field_set:Proto.Protocol.Header.uid)
}

// optional sint64 pid = 5;
inline void Header::clear_pid() {
  pid_ = GOOGLE_LONGLONG(0);
}
inline ::google::protobuf::int64 Header::pid() const {
  // @@protoc_insertion_point(field_get:Proto.Protocol.Header.pid)
  return pid_;
}
inline void Header::set_pid(::google::protobuf::int64 value) {
  
  pid_ = value;
  // @@protoc_insertion_point(field_set:Proto.Protocol.Header.pid)
}

// optional sint32 code = 6;
inline void Header::clear_code() {
  code_ = 0;
}
inline ::google::protobuf::int32 Header::code() const {
  // @@protoc_insertion_point(field_get:Proto.Protocol.Header.code)
  return code_;
}
inline void Header::set_code(::google::protobuf::int32 value) {
  
  code_ = value;
  // @@protoc_insertion_point(field_set:Proto.Protocol.Header.code)
}

// optional uint32 rpcId = 7;
inline void Header::clear_rpcid() {
  rpcid_ = 0u;
}
inline ::google::protobuf::uint32 Header::rpcid() const {
  // @@protoc_insertion_point(field_get:Proto.Protocol.Header.rpcId)
  return rpcid_;
}
inline void Header::set_rpcid(::google::protobuf::uint32 value) {
  
  rpcid_ = value;
  // @@protoc_insertion_point(field_set:Proto.Protocol.Header.rpcId)
}

// optional uint32 packetId = 8;
inline void Header::clear_packetid() {
  packetid_ = 0u;
}
inline ::google::protobuf::uint32 Header::packetid() const {
  // @@protoc_insertion_point(field_get:Proto.Protocol.Header.packetId)
  return packetid_;
}
inline void Header::set_packetid(::google::protobuf::uint32 value) {
  
  packetid_ = value;
  // @@protoc_insertion_point(field_set:Proto.Protocol.Header.packetId)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace Protocol
}  // namespace Proto

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::Proto::Protocol::PacketType> : ::google::protobuf::internal::true_type {};
template <> struct is_proto_enum< ::Proto::Protocol::BattlePacketType> : ::google::protobuf::internal::true_type {};
template <> struct is_proto_enum< ::Proto::Protocol::RpcType> : ::google::protobuf::internal::true_type {};

}  // namespace protobuf
}  // namespace google
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_RpcProtocol_2eproto__INCLUDED
