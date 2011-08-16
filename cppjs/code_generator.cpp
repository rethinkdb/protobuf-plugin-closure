// Copyright (c) 2010-2011 SameGoal LLC.
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

#include "cppjs/code_generator.h"

#include <string>
#include <iostream>  // NOLINT
#include <sstream>  // NOLINT

#include "google/protobuf/descriptor.h"
#include "google/protobuf/io/printer.h"
#include "google/protobuf/io/zero_copy_stream.h"

namespace sg {
namespace protobuf {
namespace cppjs {

CodeGenerator::CodeGenerator(const std::string &name)
    : name_(name) {}

CodeGenerator::~CodeGenerator() {}

bool CodeGenerator::HeaderFile(
    const std::string &output_h_file_name,
    const std::string &class_scope,
    google::protobuf::compiler::OutputDirectory *output_directory,
    std::string *error) const {
  google::protobuf::internal::scoped_ptr<
    google::protobuf::io::ZeroCopyOutputStream> output_h(
        output_directory->OpenForInsert(output_h_file_name,
                                        class_scope));
  google::protobuf::io::Printer h_printer(output_h.get(), '$');
  h_printer.Print(
      "bool SerializeToJsOstream(\n"
      "    const sg::protobuf::cppjs::JsFormat &fmt,\n"
      "    std::ostream *output) const;\n"
      "\n");
  h_printer.Print(
      "bool ParseFromJsIstream(\n"
      "    const sg::protobuf::cppjs::JsFormat &fmt,\n"
      "    std::istream *input);\n"
      "\n");

  if (h_printer.failed()) {
    *error = "CppJsCodeGenerator detected write error.";
    return false;
  }

  return true;
}

bool CodeGenerator::HeaderFileIncludes(
    const std::string &output_h_file_name,
    google::protobuf::compiler::OutputDirectory *output_directory,
    std::string *error) const {
  google::protobuf::internal::scoped_ptr<
    google::protobuf::io::ZeroCopyOutputStream> output_h_i(
        output_directory->OpenForInsert(output_h_file_name,
                                        "includes"));
  google::protobuf::io::Printer h_i_printer(output_h_i.get(), '$');
  h_i_printer.Print("#include <iostream>\n"
                    "#include <sstream>\n"
                    "#include \"protobuf/cppjs/lib.h\"\n");

  if (h_i_printer.failed()) {
    *error = "CppJsCodeGenerator detected write error.";
    return false;
  }

  return true;
}

bool CodeGenerator::SerializeToJsOstream(
    const std::string &output_cpp_file_name,
    const google::protobuf::Descriptor *message,
    google::protobuf::compiler::OutputDirectory *output_directory,
    std::string *error) const {
  google::protobuf::internal::scoped_ptr<
    google::protobuf::io::ZeroCopyOutputStream> output_cpp(
        output_directory->OpenForInsert(output_cpp_file_name,
                                        "namespace_scope"));
  google::protobuf::io::Printer cpp_printer(output_cpp.get(), '$');
  cpp_printer.Print(
      "bool $name$::SerializeToJsOstream(\n"
      "    const sg::protobuf::cppjs::JsFormat &fmt,\n"
      "    std::ostream *output) const {\n",
      "name", message->name());
  cpp_printer.Indent();
  cpp_printer.Print(
      "bool rtn = true;\n"
      "sg::protobuf::cppjs::JsFormat format = fmt;\n"
      "int num_prev_commas = 0;\n"
      "if (format == sg::protobuf::cppjs::ARRAY_ONE) {\n"
      "  num_prev_commas = 1;\n"
      "  format = sg::protobuf::cppjs::ARRAY;\n"
      "}\n"
      "bool prev_fields = false;\n"
      "// TODO(ahochhaus): fix bogus access to avoid compiler warnings\n"
      "++num_prev_commas;\n"
      "--num_prev_commas;\n"
      "prev_fields = !!prev_fields;\n"
      "if (format == sg::protobuf::cppjs::ARRAY) {\n"
      "  *output << \"[\";\n"
      "} else {\n"
      "  *output << \"{\";\n"
      "}\n"
      "\n");

  for (int j = 0; j < message->field_count(); ++j) {
    const google::protobuf::FieldDescriptor *field = message->field(j);
    std::ostringstream number_minus_one, number;
    number_minus_one << (field->number() - 1);
    number << field->number();
    if (field->label() !=
        google::protobuf::FieldDescriptor::LABEL_REPEATED) {
      cpp_printer.Print("// $name$\n"
                        "if (has_$name$()) {\n",
                        "name", field->lowercase_name());
    } else {
      cpp_printer.Print("// $name$\n"
                        "if (this->$name$_size() > 0) {\n",
                        "name", field->lowercase_name());
    }
    cpp_printer.Indent();
    cpp_printer.Print("if (format == sg::protobuf::cppjs::ARRAY) {\n"
                      "  while (num_prev_commas <= $number_minus_one$) {\n"
                      "    *output << \",\";\n"
                      "    ++num_prev_commas;\n"
                      "  }\n"
                      "} else if (prev_fields) {\n"
                      "  *output << \",\";\n"
                      "}\n",
                      "number_minus_one", number_minus_one.str());
    cpp_printer.Print("if (format == sg::protobuf::cppjs::NAME) {\n"
                      "  *output << \"\\\"$name$\\\":\";\n"
                      "} else if (format == sg::protobuf::cppjs::TAG) {\n"
                      "  *output << \"\\\"$id$\\\":\";\n"
                      "}\n",
                      "name", field->lowercase_name(),
                      "id", number.str());

    if (field->label() ==
        google::protobuf::FieldDescriptor::LABEL_REPEATED) {
      cpp_printer.Print("*output << \"[\";\n");
    }

    if (field->type() == google::protobuf::FieldDescriptor::TYPE_BOOL) {
      if (field->label() !=
          google::protobuf::FieldDescriptor::LABEL_REPEATED) {
        cpp_printer.Print(
            "if (this->$name$()) {\n"
            "  *output << \"true\";\n"
            "} else {\n"
            "  *output << \"false\";\n"
            "}\n",
            "name", field->lowercase_name());
      } else {
        cpp_printer.Print(
            "for (int i = 0; i < this->$name$_size(); ++i) {\n"
            "  if (this->$name$(i)) {\n"
            "    *output << \"true\";\n"
            "  } else {\n"
            "    *output << \"false\";\n"
            "  }\n"
            "  if (i != this->$name$_size() - 1) *output << \",\";\n"
            "}\n",
            "name", field->lowercase_name());
      }
    } else if (
        field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES ||
        field->type() == google::protobuf::FieldDescriptor::TYPE_STRING) {
      if (field->label() !=
          google::protobuf::FieldDescriptor::LABEL_REPEATED) {
        cpp_printer.Print(
            "*output << \"'\"\n"
            "        << sg::protobuf::cppjs::JsEscape(this->$name$())\n"
            "        << \"'\";\n",
            "name", field->lowercase_name());
      } else {
        cpp_printer.Print(
            "for (int i = 0; i < this->$name$_size(); ++i) {\n"
            "  *output << \"'\"\n"
            "          << sg::protobuf::cppjs::JsEscape(this->$name$(i))\n"
            "          << \"'\";\n"
            "  if (i != this->$name$_size() - 1) *output << \",\";\n"
            "}\n",
            "name", field->lowercase_name());
      }
    } else if (
        field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP ||
        field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
      if (field->label() !=
          google::protobuf::FieldDescriptor::LABEL_REPEATED) {
        cpp_printer.Print(
            "if (!this->$name$().SerializeToJsOstream(fmt, output)) {\n"
            "  rtn = false;\n"
            "}\n",
            "name", field->lowercase_name());
      } else {
        cpp_printer.Print(
            "for (int i = 0; i < this->$name$_size(); ++i) {\n"
            "  if (!this->$name$(i).SerializeToJsOstream(fmt, output)) {\n"
            "    rtn = false;\n"
            "  }\n"
            "  if (i != this->$name$_size() - 1) *output << \",\";\n"
            "}\n",
            "name", field->lowercase_name());
      }
    } else {
      if (field->label() !=
          google::protobuf::FieldDescriptor::LABEL_REPEATED) {
        cpp_printer.Print("*output << this->$name$();\n",
                          "name", field->lowercase_name());
      } else {
        cpp_printer.Print(
            "for (int i = 0; i < this->$name$_size(); ++i) {\n"
            "  *output << this->$name$(i);\n"
            "  if (i != this->$name$_size() - 1) *output << \",\";\n"
            "}\n",
            "name", field->lowercase_name());
      }
    }

    if (field->label() ==
        google::protobuf::FieldDescriptor::LABEL_REPEATED) {
      cpp_printer.Print("*output << \"]\";\n");
    }

    cpp_printer.Print("prev_fields = true;\n");
    cpp_printer.Outdent();
    cpp_printer.Print("}\n"
                      "\n");
  }
  // TODO(ahochhaus): Serialize unknown fields.
  cpp_printer.Print("if (format == sg::protobuf::cppjs::ARRAY) {\n"
                    "  *output << \"]\";\n"
                    "} else {\n"
                    "  *output << \"}\";\n"
                    "}\n"
                    "return rtn;\n");
  cpp_printer.Outdent();
  cpp_printer.Print("}\n"
                    "\n");

  if (cpp_printer.failed()) {
    *error = "CppJsCodeGenerator detected write error.";
    return false;
  }

  return true;
}

bool CodeGenerator::ParseFromJsIstream(
    const std::string &output_cpp_file_name,
    const google::protobuf::Descriptor *message,
    google::protobuf::compiler::OutputDirectory *output_directory,
    std::string *error) const {
  google::protobuf::internal::scoped_ptr<
    google::protobuf::io::ZeroCopyOutputStream> output_cpp(
        output_directory->OpenForInsert(output_cpp_file_name,
                                        "namespace_scope"));
  google::protobuf::io::Printer cpp_printer(output_cpp.get(), '$');

  cpp_printer.Print(
      "bool $name$::ParseFromJsIstream(\n"
      "    const sg::protobuf::cppjs::JsFormat &fmt,\n"
      "    std::istream *input) {\n",
      "name", message->name());
  cpp_printer.Indent();
  cpp_printer.Print(
      "sg::protobuf::cppjs::JsFormat format = fmt;\n"
      "int prev_tag_num = -1;\n"
      "if (format == sg::protobuf::cppjs::ARRAY_ONE) {\n"
      "  prev_tag_num = 0;\n"
      "  format = sg::protobuf::cppjs::ARRAY;\n"
      "}\n"
      "if (format == sg::protobuf::cppjs::ARRAY) {\n"
      "  if (!sg::protobuf::cppjs::ReadChar('[', input)) {\n"
      "    return false;\n"
      "  }\n"
      "} else {\n"
      "  if (!sg::protobuf::cppjs::ReadChar('{', input)) {\n"
      "    return false;\n"
      "  }\n"
      "}\n"
      "\n"
      "uint32_t tag;\n"
      "while (sg::protobuf::cppjs::ReadTag(format,\n"
      "                                    prev_tag_num,\n"
      "                                    input,\n"
      "                                    &tag)) {\n"
      "  prev_tag_num = tag;\n"
      "  if (tag == 0) {\n"
      "    // message is well formed.\n"
      "    break;\n"
      "  }\n",
      "name", message->name());
  cpp_printer.Indent();
  if (message->field_count() > 0) {
    cpp_printer.Print("std::stringstream value;\n"
                      "switch (tag) {\n");
  }
  cpp_printer.Indent();

  for (int j = 0; j < message->field_count(); ++j) {
    const google::protobuf::FieldDescriptor *field = message->field(j);
    std::ostringstream number;
    number << field->number();
    cpp_printer.Print("// $name$\n"
                      "case $number$:\n",
                      "number", number.str(),
                      "name", field->lowercase_name());
    cpp_printer.Indent();

    if (field->label() ==
        google::protobuf::FieldDescriptor::LABEL_REPEATED) {
      cpp_printer.Print(
          "if (!sg::protobuf::cppjs::ReadChar('[', input)) {\n"
          "  return false;\n"
          "}\n");
    }

    if (field->type() == google::protobuf::FieldDescriptor::TYPE_BOOL) {
      if (field->label() !=
          google::protobuf::FieldDescriptor::LABEL_REPEATED) {
        cpp_printer.Print(
            "if (!sg::protobuf::cppjs::ReadValue(format, input, &value)) {\n"
            "  return false;\n"
            "}\n"
            "if (value.str() == \"true\") {\n"
            "  this->set_$name$(true);\n"
            "} else if (value.str() == \"false\") {\n"
            "  this->set_$name$(false);\n"
            "} else {\n"
            "  return false;\n"
            "}\n",
            "name", field->lowercase_name());
      } else {
        cpp_printer.Print(
            "while (sg::protobuf::cppjs::ReadValue(format, input, &value)) {\n"
            "  if (value.str().empty()) {\n"
            "    break;\n"
            "  }\n"
            "  if (value.str() == \"true\") {\n"
            "    this->add_$name$(true);\n"
            "  } else if (value.str() == \"false\") {\n"
            "    this->add_$name$(false);\n"
            "  } else {\n"
            "    return false;\n"
            "  }\n"
            "}\n"
            "if (!value.str().empty()) {\n"
            "  return false;\n"
            "}\n",
            "name", field->lowercase_name());
      }
    } else if (
        field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES ||
        field->type() == google::protobuf::FieldDescriptor::TYPE_STRING) {
      if (field->label() !=
          google::protobuf::FieldDescriptor::LABEL_REPEATED) {
        cpp_printer.Print(
            "if (!sg::protobuf::cppjs::ReadValue(format, input, &value)) {\n"
            "  return false;\n"
            "}\n"
            "{\n"
            "  std::stringstream tmp;\n"
            // 0 = leading, 1 = single quote, 2 = single quote escape
            // 3 = double quote, 4 = double quote escape, 5 = trailing
            "  int state = 0;\n"
            "  while(value.peek() && !value.eof()) {\n"
            "    if (state == 0) {\n"
            "      if (value.peek() == '\\\'') {\n"
            "        value.get();\n"
            "        state = 1;\n"
            "        continue;\n"
            "      } else if (value.peek() == '\\\"') {\n"
            "        value.get();\n"
            "        state = 3;\n"
            "        continue;\n"
            "      } else if (value.peek() == ' ' ||\n"
            "                 value.peek() == '\\t' ||\n"
            "                 value.peek() == '\\r' ||\n"
            "                 value.peek() == '\\n') {\n"
            "        value.get();\n"
            "        continue;\n"
            "      }\n"
            "      return false;\n"
            "    } else if (state == 1) {\n"
            "      if (value.peek() == '\\\'') {\n"
            "        value.get();\n"
            "        state = 5;\n"
            "        continue;\n"
            "      } else if (value.peek() == '\\\\') {\n"
            "        value.get();\n"
            "        state = 2;\n"
            "        continue;\n"
            "      }\n"
            "    } else if (state == 2) {\n"
            "      state = 1;\n"
            "    } else if (state == 3) {\n"
            "      if (value.peek() == '\\\"') {\n"
            "        value.get();\n"
            "        state = 5;\n"
            "        continue;\n"
            "      } else if (value.peek() == '\\\\') {\n"
            "        value.get();\n"
            "        state = 4;\n"
            "        continue;\n"
            "      }\n"
            "    } else if (state == 4) {\n"
            "      state = 3;\n"
            "    } else if (state == 5) {\n"
            "      if (value.peek() == ' ' ||\n"
            "          value.peek() == '\\t' ||\n"
            "          value.peek() == '\\r' ||\n"
            "          value.peek() == '\\n') {\n"
            "        value.get();\n"
            "        continue;\n"
            "      }\n"
            "      return false;\n"
            "    } else {\n"
            "      return false;\n"
            "    }\n"
            "    tmp << (char)value.get();\n"
            "  }\n"
            "  if (state != 5) {\n"
            "    return false;\n"
            "  }\n"
            "  this->set_$name$(tmp.str());\n"
            "}\n",
            "name", field->lowercase_name());
      } else {
        cpp_printer.Print(
            "while (sg::protobuf::cppjs::ReadValue(format, input, &value)) {\n"
            "  if (value.str().empty()) {\n"
            "    break;\n"
            "  }\n"
            "  {\n"
            "    std::stringstream tmp;\n"
            // 0 = leading, 1 = single quote, 2 = single quote escape
            // 3 = double quote, 4 = double quote escape, 5 = trailing
            "    int state = 0;\n"
            "    while(value.peek() && !value.eof()) {\n"
            "      if (state == 0) {\n"
            "        if (value.peek() == '\\\'') {\n"
            "          value.get();\n"
            "          state = 1;\n"
            "          continue;\n"
            "        } else if (value.peek() == '\\\"') {\n"
            "          value.get();\n"
            "          state = 3;\n"
            "          continue;\n"
            "        } else if (value.peek() == ' ' ||\n"
            "                   value.peek() == '\\t' ||\n"
            "                   value.peek() == '\\r' ||\n"
            "                   value.peek() == '\\n') {\n"
            "          value.get();\n"
            "          continue;\n"
            "        }\n"
            "        return false;\n"
            "      } else if (state == 1) {\n"
            "        if (value.peek() == '\\\'') {\n"
            "          value.get();\n"
            "          state = 5;\n"
            "          continue;\n"
            "        } else if (value.peek() == '\\\\') {\n"
            "          value.get();\n"
            "          state = 2;\n"
            "          continue;\n"
            "        }\n"
            "      } else if (state == 2) {\n"
            "        state = 1;\n"
            "      } else if (state == 3) {\n"
            "        if (value.peek() == '\\\"') {\n"
            "          value.get();\n"
            "          state = 5;\n"
            "          continue;\n"
            "        } else if (value.peek() == '\\\\') {\n"
            "          value.get();\n"
            "          state = 4;\n"
            "          continue;\n"
            "        }\n"
            "      } else if (state == 4) {\n"
            "        state = 3;\n"
            "      } else if (state == 5) {\n"
            "        if (value.peek() == ' ' ||\n"
            "            value.peek() == '\\t' ||\n"
            "            value.peek() == '\\r' ||\n"
            "            value.peek() == '\\n') {\n"
            "          value.get();\n"
            "          continue;\n"
            "        }\n"
            "        return false;\n"
            "      } else {\n"
            "        return false;\n"
            "      }\n"
            "      tmp << (char)value.get();\n"
            "    }\n"
            "    if (state != 5) {\n"
            "      return false;\n"
            "    }\n"
            "    this->add_$name$(tmp.str());\n"
            "  }\n"
            "}\n"
            "if (!value.str().empty()) {\n"
            "  return false;\n"
            "}\n",
            "name", field->lowercase_name());
      }
    } else if (
        field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP ||
        field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
      if (field->label() !=
          google::protobuf::FieldDescriptor::LABEL_REPEATED) {
        cpp_printer.Print(
            "if (!sg::protobuf::cppjs::ReadValue(format, input, &value)) {\n"
            "  return false;\n"
            "}\n"
            "if (!this->mutable_$name$()->ParseFromJsIstream(fmt,\n"
            "                                                &value)) {\n"
            "  return false;\n"
            "}\n",
            "name", field->lowercase_name());
      } else {
        cpp_printer.Print(
            "while (sg::protobuf::cppjs::ReadValue(format,\n"
            "                                      input,\n"
            "                                      &value)) {\n"
            "  if (value.str().empty()) {\n"
            "    break;\n"
            "  }\n"
            "  if (!this->add_$name$()->ParseFromJsIstream(fmt,\n"
            "                                              &value)) {\n"
            "    return false;\n"
            "  }\n"
            "}\n"
            "if (!value.str().empty()) {\n"
            "  return false;\n"
            "}\n",
            "name", field->lowercase_name());
      }
    } else {
      std::string type = "ERROR";
      std::string type_cast = "tmp";
      if (field->type() == google::protobuf::FieldDescriptor::TYPE_DOUBLE) {
        type = "double";
      } else if (field->type() ==
                 google::protobuf::FieldDescriptor::TYPE_FLOAT) {
        type = "float";
      } else if (field->type() ==
                 google::protobuf::FieldDescriptor::TYPE_INT64 ||
                 field->type() ==
                 google::protobuf::FieldDescriptor::TYPE_SINT64 ||
                 field->type() ==
                 google::protobuf::FieldDescriptor::TYPE_SFIXED64) {
        type = "int64_t";
      } else if (field->type() ==
                 google::protobuf::FieldDescriptor::TYPE_UINT64 ||
                 field->type() ==
                 google::protobuf::FieldDescriptor::TYPE_FIXED64) {
        type = "uint64_t";
      } else if (field->type() ==
                 google::protobuf::FieldDescriptor::TYPE_INT32 ||
                 field->type() ==
                 google::protobuf::FieldDescriptor::TYPE_SINT32 ||
                 field->type() ==
                 google::protobuf::FieldDescriptor::TYPE_SFIXED32) {
        type = "int32_t";
      } else if (field->type() ==
                 google::protobuf::FieldDescriptor::TYPE_UINT32 ||
                 field->type() ==
                 google::protobuf::FieldDescriptor::TYPE_FIXED32) {
        type = "uint32_t";
      } else if (field->type() ==
                 google::protobuf::FieldDescriptor::TYPE_ENUM) {
        type = "int";
        std::string cpp_enum_type = field->enum_type()->full_name();
        while (cpp_enum_type.find(".") != std::string::npos) {
          cpp_enum_type = cpp_enum_type.replace(
              cpp_enum_type.find("."), 1, "::");
        }
        type_cast = "static_cast< " + cpp_enum_type + " > (tmp)";
      }

      if (field->label() !=
          google::protobuf::FieldDescriptor::LABEL_REPEATED) {
        cpp_printer.Print(
            "if (!sg::protobuf::cppjs::ReadValue(format, input, &value)) {\n"
            "  return false;\n"
            "}\n"
            "{\n"
            "  $cpp_type$ tmp;\n"
            "  value >> tmp;\n",
            "cpp_type", type);
        cpp_printer.Print(
            "  this->set_$name$($type_cast$);\n"
            "}\n",
            "name", field->lowercase_name(),
            "type_cast", type_cast);
      } else {
        cpp_printer.Print(
            "while (sg::protobuf::cppjs::ReadValue(format, input, &value)) {\n"
            "  if (value.str().empty()) {\n"
            "    break;\n"
            "  }\n"
            "  {\n"
            "    $cpp_type$ tmp;\n"
            "    value >> tmp;\n",
            "cpp_type", type);
        cpp_printer.Print(
            "    this->add_$name$($type_cast$);\n"
            "  }\n"
            "}\n"
            "if (!value.str().empty()) {\n"
            "  return false;\n"
            "}\n",
            "name", field->lowercase_name(),
            "type_cast", type_cast);
      }
    }

    if (field->label() ==
        google::protobuf::FieldDescriptor::LABEL_REPEATED) {
      cpp_printer.Print(
          "if (!sg::protobuf::cppjs::ReadChar(']', input)) {\n"
          "  return false;\n"
          "}\n"
          "if (input->peek() && !input->eof() && input->peek() == ',') {\n"
          "  input->get();\n"
          "}\n");
    }

    cpp_printer.Print("break;\n");
    cpp_printer.Outdent();
    cpp_printer.Print("\n");
  }

  // TODO(ahochhaus): Deserialize unknown fields.

  cpp_printer.Outdent();
  if (message->field_count() > 0) {
    cpp_printer.Print("}\n");
  }
  cpp_printer.Outdent();
  cpp_printer.Print("}\n"
                    "if (tag != 0) {\n"
                    "  return false;\n"
                    "}\n"
                    "\n"
                    "if (format == sg::protobuf::cppjs::ARRAY) {\n"
                    "  if (!sg::protobuf::cppjs::ReadChar(']', input)) {\n"
                    "    return false;\n"
                    "  }\n"
                    "} else {\n"
                    "  if (!sg::protobuf::cppjs::ReadChar('}', input)) {\n"
                    "    return false;\n"
                    "  }\n"
                    "}\n"
                    "sg::protobuf::cppjs::LStripWhiteSpace(input);\n"
                    "if (input->peek() && !input->eof()) {\n"
                    "  return false;\n"
                    "}\n"
                    "return true;\n");

  cpp_printer.Outdent();
  cpp_printer.Print("}\n"
                    "\n");

  if (cpp_printer.failed()) {
    *error = "CppJsCodeGenerator detected write error.";
    return false;
  }

  return true;
}  // NOLINT

bool CodeGenerator::Generate(
    const google::protobuf::FileDescriptor *file,
    const std::string &parameter,
    google::protobuf::compiler::OutputDirectory *output_directory,
    std::string *error) const {

  std::string output_h_file_name = file->name();
  std::size_t loc = output_h_file_name.rfind(".");
  output_h_file_name.erase(loc, output_h_file_name.length() - loc);
  std::string output_cpp_file_name = output_h_file_name;
  output_h_file_name.append(".pb.h");
  output_cpp_file_name.append(".pb.cc");

  bool write = CodeGenerator::HeaderFileIncludes(output_h_file_name,
                                                 output_directory,
                                                 error);
  if (!write) {
    return false;
  }

  for (int i = 0; i < file->message_type_count(); ++i) {
    const google::protobuf::Descriptor *message = file->message_type(i);
    const std::string class_scope = "class_scope:" + message->full_name();

    CodeGenerator::HeaderFile(output_h_file_name,
                              class_scope,
                              output_directory,
                              error);
    if (!write) {
      return false;
    }

    CodeGenerator::SerializeToJsOstream(output_cpp_file_name,
                                        message,
                                        output_directory,
                                        error);
    if (!write) {
      return false;
    }

    CodeGenerator::ParseFromJsIstream(output_cpp_file_name,
                                      message,
                                      output_directory,
                                      error);
    if (!write) {
      return false;
    }
  }

  return true;
}

}  // namespace cppjs
}  // namespace protobuf
}  // namespace sg
