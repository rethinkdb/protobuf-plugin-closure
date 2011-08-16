// Copyright (c) 2011 SameGoal LLC.
// All Rights Reserved.
// Author: Andy Hochhaus

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "js/int64_encoding.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::EnumDescriptor* Int64Encoding_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_int64_5fencoding_2eproto() {
  protobuf_AddDesc_int64_5fencoding_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "int64_encoding.proto");
  GOOGLE_CHECK(file != NULL);
  Int64Encoding_descriptor_ = file->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_int64_5fencoding_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
}

}  // namespace

void protobuf_ShutdownFile_int64_5fencoding_2eproto() {
}

void protobuf_AddDesc_int64_5fencoding_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::protobuf_AddDesc_google_2fprotobuf_2fdescriptor_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n protobuf/js/int64_encoding.proto\032 goog"
    "le/protobuf/descriptor.proto:\?\n\026int64_en"
    "code_as_number\022\035.google.protobuf.FieldOp"
    "tions\030\321\206\003 \001(\010", 133);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "int64_encoding.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::ExtensionSet::RegisterEnumExtension(
    &::google::protobuf::FieldOptions::default_instance(),
    50001, 14, false, false,
    &Int64Encoding_IsValid);
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_int64_5fencoding_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_int64_5fencoding_2eproto {
  StaticDescriptorInitializer_int64_5fencoding_2eproto() {
    protobuf_AddDesc_int64_5fencoding_2eproto();
  }
} static_descriptor_initializer_int64_5fencoding_2eproto_;

const ::google::protobuf::EnumDescriptor* Int64Encoding_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Int64Encoding_descriptor_;
}
bool Int64Encoding_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}

::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::FieldOptions,
    ::google::protobuf::internal::EnumTypeTraits< Int64Encoding, Int64Encoding_IsValid>, 14, false >
  jstype(kJstypeFieldNumber, static_cast< Int64Encoding >(0));

// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
