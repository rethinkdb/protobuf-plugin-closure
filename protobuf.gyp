# Copyright (c) 2011 SameGoal LLC.
# All Rights Reserved.
# Author: Andy Hochhaus <ahochhaus@samegoal.com>

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

{
  'targets': [
    {
      'target_name': 'protobuf_js_pb',
      'type': '<(library)',
      'dependencies': [
        '../third-party/protobuf/protobuf.gyp:protoc#host',
      ],
      'sources': [
        'js/javascript_package.proto',
        'js/int64_encoding.proto',
      ],
    },
    {
      'target_name': 'protoc-gen-js',
      'type': 'executable',
      'include_dirs': [
        '.',
      ],
      'dependencies': [
        'protobuf_js_pb',
        '../third-party/protobuf/protobuf.gyp:protobuf_full_use_sparingly',
      ],
      'sources': [
        'js/code_generator.cpp',
        'js/protoc_gen_js.cpp',
      ],
      'conditions': [
        ['OS=="linux"', {
          'ldflags': [
            '-pthread',
          ],
        }],
      ],
    },
    {
      'target_name': 'protoc-gen-cppjs',
      'type': 'executable',
      'include_dirs': [
        '.',
      ],
      'dependencies': [
        '../third-party/protobuf/protobuf.gyp:protobuf_full_use_sparingly',
      ],
      'sources': [
        'cppjs/code_generator.cpp',
        'cppjs/protoc_gen_cppjs.cpp',
      ],
      'conditions': [
        ['OS=="linux"', {
          'ldflags': [
            '-pthread',
          ],
        }],
      ],
    },
    {
      'target_name': 'test_pb',
      'type': '<(library)',
      'dependencies': [
        'protobuf_js_pb',
        'protoc-gen-js',
        'protoc-gen-cppjs',
        '../third-party/protobuf/protobuf.gyp:closure_protoc',
      ],
      'sources': [
        'js/package_test.proto',
        'js/test.proto',
      ],
    },
    {
      'target_name': 'cppjs_test',
      'type': 'executable',
      'dependencies': [
        'test_pb',
        '../third-party/googletest/gtest.gyp:gtest',
        '../third-party/google-glog/glog.gyp:glog',
        '../third-party/protobuf/protobuf.gyp:protobuf_full_use_sparingly',
      ],
      'include_dirs' : [
        '..',
      ],
      'sources': [
        'cppjs/cppjs_test.cpp',
      ],
    },
  ],
}
