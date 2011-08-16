// Copyright (c) 2010 SameGoal LLC.
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

#ifndef PROTOBUF_CPPJS_LIB_H_
#define PROTOBUF_CPPJS_LIB_H_

#include <string>
#include <iostream>  // NOLINT

#include "google/protobuf/stubs/common.h"

namespace sg {
namespace protobuf {
namespace cppjs {

enum JsFormat {
  TAG,
  NAME,
  ARRAY,
  ARRAY_ONE  // start tag numbering from one instead of zero
};

int JsEscapeInternal(const char *src, int src_len, char *dest, int dest_len);
std::string JsEscape(const std::string &src);

void LStripWhiteSpace(std::istream *input);
bool ReadChar(const char c, std::istream *input);
bool ReadTag(const JsFormat &format,
             uint32_t prev_tag_num,
             std::istream *input,
             uint32_t *tag_num);
bool ReadValue(const JsFormat &format,
               std::istream *input,
               std::stringstream *value);

}  // namespace cppjs
}  // namespace protobuf
}  // namespace sg

#endif  // PROTOBUF_CPPJS_LIB_H_
