# ===-- HashTableForProperties.py - Create a Hash Table for C from a Configuration JSON --------------*- Python -*-=== #
#
# Copyright (c) 2020 Oever González
#
#  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
#  the License. You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
#  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
#  specific language governing permissions and limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0
#
# ===--------------------------------------------------------------------------------------------------------------=== #
# /
# / \file
# / This file will generate a C source code file which contains a Hash Table, where the hash represents a string to a
# / property in a JSON object; and whose value is a pointer to the value of the property. This will implement a kind of
# / lookup table, where the C program can calculate the hash of the property and use it as a index to the table and get
# / the pointer.
# /
# ===--------------------------------------------------------------------------------------------------------------=== #

import argparse
import io
import json
import logging
import sys

bits = 4
buffer_size = 64 * 1024  # 64kb
collision_foresee = 1
default_db_filename = 'properties.json'
default_header_filename = 'JsonPropertyHashTable.h'
default_source_filename = 'JsonPropertyHashTable.c'
default_template_filename = 'template.h'
max_64bit = 0xFFFFFFFFFFFFFFFF
parsed_args = None
pattern_64bit = 0xAAAAAAAAAAAAAAAA
program_parser = None
this_logger = logging.getLogger(__name__)


def parse_args(args):
    global buffer_size
    global default_db_filename
    global default_header_filename
    global default_source_filename
    global default_template_filename
    global parsed_args
    global program_parser
    parser = argparse.ArgumentParser(
            description='Creates a C source code file which contains a lookup table intended to be used as a '
                        'dictionary from properties in a JSON file. The program will calculate a hash which will be '
                        'the index of the array. The C program can calculate the hash and get the information from the '
                        'array, which is a pointer to the property string and it\'s value.',
            epilog='This file requires a template header, which will be included inside both the C header and the C '
                   'source file. The result will be in the provided output directory, which is one for the header and '
                   'other for the C source file. Those files can be included in a build system, just before this '
                   'program generates them.')
    parser.add_argument('-j', '--json-file',
                        action='store',
                        default=default_db_filename,
                        help='This is the input file that will be used to generate the lookup table based on the JSON '
                             'properties of the objects in this file.',
                        type=argparse.FileType(bufsize=buffer_size))
    parser.add_argument('-t', '--template',
                        action='store',
                        default=default_template_filename,
                        help='The template will be inserted at the beginning of the header and the source code.',
                        type=argparse.FileType(bufsize=buffer_size))
    parser.add_argument('-e', '--header',
                        action='store',
                        default=default_header_filename,
                        help='The output file where the header will be generated.',
                        type=argparse.FileType(mode='w', bufsize=buffer_size))
    parser.add_argument('-s', '--source',
                        action='store',
                        default=default_source_filename,
                        help='The output file where the source code will be generated.',
                        type=argparse.FileType(mode='w', bufsize=buffer_size))
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help='Using this flag will print debug messages from the logger to the console by default.')
    program_parser = parser
    parsed_args = parser.parse_args(args)
    return parsed_args


# Extracted from:
# https://towardsdatascience.com/how-to-flatten-deeply-nested-json-objects-in-non-recursive-elegant-python-55f96533103d
def flatten_json(input_json):
    output = {}

    def flatten(json_dict, name=''):
        if type(json_dict) is dict:
            for value in json_dict:
                flatten(json_dict[value], name + value + '.')
        elif type(json_dict) is list:
            index = 0
            for value in json_dict:
                flatten(value, name + str(index) + '.')
                index += 1
        else:
            output[name[:-1]] = json_dict

    flatten(input_json)
    return output


def mask(value):
    global bits
    bit_mask = 0
    for i in range(bits):
        bit_mask = bit_mask << 1 | 1
    result = value & bit_mask
    return result


def rotate_left(value):
    global bits
    masked = mask(value)
    rotated = masked << 1
    carry = rotated >> bits
    rotated = rotated | carry
    return mask(rotated)


def calculate_hash(input):
    global pattern_64bit
    value = pattern_64bit
    ascii_bits = input.encode('ASCII')
    for character in ascii_bits:
        value = rotate_left(value)
        value ^= ~character * ~len(input)
        value = rotate_left(value)
        value ^= character * ~len(input)
    value = mask(value)
    return value


def calculate_hash2(input):
    global pattern_64bit
    value = pattern_64bit
    ascii_bits = input.encode('ASCII')
    for character in ascii_bits:
        value = rotate_left(value)
        value ^= character * ~len(input)
        value = rotate_left(value)
        value ^= ~character * ~len(input)
    value = mask(value)
    return value


def hash_foresee(key_hash, name, hashmap, key, value):
    global collision_foresee
    global this_logger
    collision_avoided = False
    lowermost = mask(key_hash - collision_foresee)
    uppermost = mask(key_hash + collision_foresee + 1)
    step = 1 if uppermost >= lowermost else -1
    for index in range(lowermost, uppermost, step):
        if index == key_hash:
            continue
        this_logger.warning("Trying to solve collision by searching around the {} hash {} in {}..."
                            .format(name, hex(key_hash), hex(index)))
        current_in_map = hashmap[index]
        if current_in_map is None:
            hashmap[index] = "{}%=%=%{}".format(key, value)
            collision_avoided = True
            this_logger.info("Collision avoided by moving hash {} ({}) at index {} ({})"
                             .format(hex(key_hash), key_hash, index, hex(index)))
            break
    return collision_avoided


def create_hashmap(args):
    global max_64bit
    global this_logger
    json_data = json.load(args.json_file)
    json_data = flatten_json(json_data)
    hashmap = [None] * (mask(max_64bit) + 1)
    for key, value in json_data.items():
        key_hash = calculate_hash(key)
        this_logger.info("Key: {}, Value: {}, Current Hash: {}".format(key, value, hex(key_hash)))
        current_in_map = hashmap[key_hash]
        if current_in_map is None:
            hashmap[key_hash] = "{}%=%=%{}".format(key, value)
            this_logger.info("Key not found in the map, key added at index {}!".format(key_hash))
        else:
            collision_avoided = False
            key_hash2 = calculate_hash2(key)
            current_in_map = hashmap[key_hash2]
            if current_in_map is None:
                this_logger.warning("Trying to use the alternative hash {} for hash {}"
                                    .format(hex(key_hash2), hex(key_hash)))
                hashmap[key_hash2] = "{}%=%=%{}".format(key, value)
                collision_avoided = True
                this_logger.info("Collision avoided by using the alternative hash".format(key_hash))
            if not collision_avoided:
                collision_avoided = hash_foresee(key_hash, 'first', hashmap, key, value)
            if not collision_avoided:
                collision_avoided = hash_foresee(key_hash2, 'second', hashmap, key, value)
            if not collision_avoided:
                this_logger.warning("Failed to solve the collision for the second hash {}".format(hex(key_hash2)))
                this_logger.error("Error: Unrecoverable collision detected...".format(len(hashmap)))
                this_logger.error("Length of the map: '{}'".format(len(hashmap)))
                this_logger.error("Key to be inserted: '{}'".format(key))
                this_logger.error("Key with the same hash: '{}'".format(hashmap[key_hash]))
                this_logger.error("Hash that collided: '{}'".format(hex(key_hash)))
                raise SystemExit(2, 'Hash collision detected.')
    return hashmap


def print_to_source(args, hashmap):
    global program_parser
    if not args.template or not args.header or not args.source:
        program_parser.error('Generating the source code requires the template and the output files.')
    with args.template as template:
        with io.StringIO("") as source:
            source.write("const struct jsonPropertyValue *const jsonParametersHashmap[] =\n\t{")
            with io.StringIO("") as variables:
                variables.write("\n#include <{}>\n\n".format(args.header.name))
                for index in range(len(hashmap)):
                    hashvalue = hashmap[index]
                    ending = ", " if index != (len(hashmap) - 1) else ""
                    if hashvalue is None:
                        source.write("NULL{}".format(ending))
                    else:
                        varname = "jsonPropertyValue" + hex(index)
                        key, val = hashvalue.split('%=%=%')
                        source.write("&{}{}".format(varname, ending))
                        variables.write("static const struct jsonPropertyValue {} = {{\n\t".format(varname))
                        variables.write("\"{}\",\n\t\"{}\"\n}};\n".format(key, val))
                source.write("};\n")
                source.seek(0)
                variables.write("\n")
                variables.seek(0)
                with args.source as real_output:
                    template.seek(0)
                    real_output.write(template.read())
                    real_output.write(variables.read())
                    real_output.write(source.read())
        with io.StringIO("") as header:
            header.write("\nstruct jsonPropertyValue {\n\tchar *name;\n\tchar *value;\n};\n")
            header.write("\nconst struct jsonPropertyValue *const jsonParametersHashmap[];\n")
            with args.header as real_output:
                template.seek(0)
                real_output.write(template.read())
                header.seek(0)
                real_output.write(header.read())


def main(args):
    global parsed_args
    global program_parser
    global this_logger
    parsed = parse_args(args)
    if program_parser is None or parsed_args is None:
        raise SystemExit(1, 'Program argument parser not set or global argument set is None.')
    if parsed.verbose:
        this_logger.setLevel(logging.DEBUG)
        this_logger.warning('Debug logging enabled')
    hashmap = create_hashmap(parsed)
    print_to_source(parsed, hashmap)


if __name__ == '__main__':
    main(sys.argv[1:])
