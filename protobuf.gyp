# Copyright (c) 2011 SameGoal LLC.
# All Rights Reserved.
# Author: Andy Hochhaus

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
      'target_name': 'protoc-gen-js',
      'type': 'executable',
      'include_dirs': [
        '.',
      ],
      'dependencies': [
        '../third-party/protobuf/protobuf.gyp:protobuf_full_use_sparingly',
      ],
      'sources': [
        'js/code_generator.cpp',
        'js/protoc-gen-js.cpp',
        'js/int64_encoding.pb.cc',
        'js/javascript_package.pb.cc',
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
        'cppjs/protoc-gen-cppjs.cpp',
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
      'target_name': 'cppjs-lib',
      'type': '<(library)',
      'include_dirs': [
        '.',
      ],
      'dependencies': [
        '../third-party/protobuf/protobuf.gyp:protobuf_lite',
      ],
      'sources': [
        'cppjs/lib.cpp',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '..',
        ],
      },
    },
  ],
}
