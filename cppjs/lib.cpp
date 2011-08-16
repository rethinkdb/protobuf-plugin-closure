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

#include "cppjs/lib.h"

#include <ctype.h>
#include <stdio.h>

#include <iostream>  // NOLINT
#include <sstream>  // NOLINT
#include <string>
#include <vector>

#include "google/protobuf/stubs/common.h"


namespace sg {
namespace protobuf {
namespace cppjs {

// Modified from google/protobuf/stubs/strutil.cc
int JsEscapeInternal(const char *src, int src_len, char *dest, int dest_len) {
  const char *src_end = src + src_len;
  int used = 0;

  for (; src < src_end; src++) {
    if (dest_len - used < 2)
      return -1;

    switch (*src) {
      case '\n': dest[used++] = '\\'; dest[used++] = 'n';  break;
      case '\r': dest[used++] = '\\'; dest[used++] = 'r';  break;
      case '\t': dest[used++] = '\\'; dest[used++] = 't';  break;
      case '\"': dest[used++] = '\\'; dest[used++] = '\"'; break;
      case '\'': dest[used++] = '\\'; dest[used++] = '\''; break;
      case '\\': dest[used++] = '\\'; dest[used++] = '\\'; break;
      default:
        if (isprint(*src)) {
          dest[used++] = *src;
          break;
        }
    }
  }

  if (dest_len - used < 1)
    return -1;

  dest[used] = '\0';
  return used;
}

std::string JsEscape(const std::string &src) {
  const int dest_length = src.size() * 2 + 1;
  google::protobuf::internal::scoped_array<char> dest(new char[dest_length]);
  const int len = JsEscapeInternal(src.data(), src.size(),
                                   dest.get(), dest_length);
  return std::string(dest.get(), len);
}

void LStripWhiteSpace(std::istream *input) {
  while (input->peek() && !input->eof()) {
    if (input->peek() != ' ' &&
        input->peek() != '\t' &&
        input->peek() != '\n' &&
        input->peek() != '\r') {
      break;
    }
    input->get();
  }
}

bool ReadChar(const char c, std::istream *input) {
  LStripWhiteSpace(input);
  if (input->peek() && input->eof()) {
    return false;
  }
  if (input->get() != c) {
    return false;
  }
  return true;
}

// return true with non-zero tag: tag sucessfully read
// return true with zero tag: no more tags to read, but message properly formed
// return false: malformed message

bool ReadTag(const JsFormat &format,
             uint32_t prev_tag_num,
             std::istream *input,
             uint32_t *tag_num) {
  if (format == sg::protobuf::cppjs::ARRAY) {
    LStripWhiteSpace(input);
    while (input->peek() && !input->eof()) {
      if (input->peek() == ']') {
        // no more tags but well formed message
        *tag_num = 0;
        return true;
      }

      if (input->peek() == ',') {
        // burn the ',' and advance (skipping the place-holder field)
        input->get();
        LStripWhiteSpace(input);
        ++prev_tag_num;
      } else if (input->peek() == 'n') {
        // burn the 'null,' and advance (skipping the place-holder field)
        input->get();  // n
        input->get();  // u
        input->get();  // l
        input->get();  // l
        LStripWhiteSpace(input);
        input->get();  // ,
        ++prev_tag_num;
      } else {
        *tag_num = prev_tag_num + 1;
        return true;
      }
    }
  } else {
    LStripWhiteSpace(input);
    if (input->peek() == '}') {
      // no more tags but well formed message
      *tag_num = 0;
      return true;
    }

    std::stringstream tag_stream;
    while (input->peek() && !input->eof()) {
      if (input->peek() == ':') {
        // burn the ':'
        input->get();

        if (format == sg::protobuf::cppjs::NAME) {
          // TODO(ahochhaus): Add support for sg::protobuf::cppjs::NAME
          // which requires mapping from the tag name to tag number.
          return false;
        } else {
          tag_stream >> *tag_num;
          if (*tag_num == 0) {
            *tag_num = 1;
            return false;
          }
          return true;
        }
      } else if (input->peek() == '"' || input->peek() == '\'') {
        // TODO(ahochhaus): Process quotes more intelligently than simply
        // dropping them.
        input->get();
        continue;
      }

      // malformed massage. found '}' before ':'
      if (input->peek() == '}') {
        *tag_num = 1;
        return false;
      }

      tag_stream << static_cast<char> (input->get());
    }
  }

  // no tags found
  *tag_num = 1;
  return false;
}

enum ParserState {
  NORMAL,
  SINGLE_QUOTED,  // single quoted string
  DOUBLE_QUOTED  // double quoted string
};

enum NestedItem {
  LIST,
  OBJECT
};

// two use cases:
// * read exactly one value followed by ',', '}' or ']'
// * read zero or more values, each followed by ',' or ']'

// return true: value successfully read
// return true with empty value: no more values to read,
//                               but message properly formed
// return false: malformed message

bool ReadValue(const JsFormat &format,
               std::istream *input,
               std::stringstream *value) {
  bool escape_next = false;
  ParserState parser_state = NORMAL;
  std::vector<NestedItem> parse_stack;
  value->str("");
  value->clear();

  LStripWhiteSpace(input);
  while (input->peek() && !input->eof()) {
    if (escape_next) {
      escape_next = false;

      switch (input->peek()) {
        case 'n':
          *value << '\n';
          input->get();
          continue;
          break;
        case 'r':
          *value << '\r';
          input->get();
          continue;
          break;
        case 't':
          *value << '\t';
          input->get();
          continue;
          break;
        case 'u':
          // TODO(ahochhaus): Parse unicode reasonably.
          input->get();  // u
          char code[5];
          code[0] = input->get();
          code[1] = input->get();
          code[2] = input->get();
          code[3] = input->get();
          code[4] = NULL;

          uint32_t char_code;
          if (sscanf(code, "%x", &char_code) != 1) {
            // TODO(ahochhaus): Error checking.
            char_code = 0;
          }

          *value << static_cast<char>(char_code);

          continue;
          break;
      }
    } else if (input->peek() == '\\') {
      escape_next = true;
    } else {
      switch (parser_state) {
        case NORMAL:
          if (input->peek() == '[') {
            parse_stack.push_back(LIST);
          } else if (input->peek() == '{') {
            parse_stack.push_back(OBJECT);
          } else if (input->peek() == '\'') {
            parser_state = SINGLE_QUOTED;
          } else if (input->peek() == '\"') {
            parser_state = DOUBLE_QUOTED;
          } else if (parse_stack.size() > 0) {
            if (input->peek() == ']') {
              if (parse_stack.back() != LIST) {
                return false;
              }
              parse_stack.pop_back();
            } else if (input->peek() == '}') {
              if (parse_stack.back() != OBJECT) {
                return false;
              }
              parse_stack.pop_back();
            }
          } else {
            if (input->peek() == ',') {
              // found end of value-> burn ',' and return.
              input->get();
              return true;

            } else if (input->peek() == ']') {
              // don't check format != sg::protobuf::cppjs::ARRAY because we
              // could be in a repeated item.
              return true;
            } else if (input->peek() == '}') {
              if (format == sg::protobuf::cppjs::ARRAY) {
                return false;
              }
              return true;
            }
          }
          break;
        case SINGLE_QUOTED:
          if (input->peek() == '\'') {
            parser_state = NORMAL;
          }
          break;
        case DOUBLE_QUOTED:
          if (input->peek() == '\"') {
            parser_state = NORMAL;
          }
          break;
      }
    }

    *value << static_cast<char> (input->get());
  }

  return false;
}

}  // namespace cppjs
}  // namespace protobuf
}  // namespace sg
