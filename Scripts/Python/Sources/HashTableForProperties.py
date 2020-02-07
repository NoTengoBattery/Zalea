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
import json
import logging
import sys

bits = 8
buffer_size = 64 * 1024  # 64kb
collision_foresee = 2
default_db_filename = 'properties.json'
max_64bit = 0xFFFFFFFFFFFFFFFF
pattern_64bit = 0xAAAAAAAAAAAAAAAA
parsed_args = None
program_parser = None
this_logger = logging.getLogger(__name__)


def parse_args(args):
    global buffer_size
    global default_db_filename
    global program_parser
    global parsed_args
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
                        metavar='json_db',
                        type=argparse.FileType(bufsize=buffer_size))
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
    global bits
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


def dump_db(args):
    global collision_foresee
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
            hashmap[key_hash] = "{}={}".format(key, value)
            this_logger.info("Key not found in the map, key added at index {}".format(key_hash))
        else:
            collision_avoided = False
            lowermost = mask(key_hash - collision_foresee)
            uppermost = mask(key_hash + collision_foresee + 1)
            step = 1 if uppermost >= lowermost else -1
            for index in range(lowermost, uppermost, step):
                this_logger.info("Trying to solve collision, attempting to insert the key at index {}"
                                 .format(index))
                current_in_map = hashmap[index]
                if current_in_map is None:
                    hashmap[index] = "{}={}".format(key, value)
                    collision_avoided = True
                    this_logger.info("Collision avoided by moving hash {} ({}) at index {} ({})"
                                     .format(hex(key_hash), key_hash, index, hex(index)))
                    break
                else:
                    collision_avoided = False
                    this_logger.warning("Cannot solve the collision for hash {} because the index {} is already full"
                                        .format(hex(key_hash), hex(index)))
            if not collision_avoided:
                this_logger.error("Length of the map: '{}'".format(len(hashmap)))
                this_logger.error("Key to be inserted: '{}'".format(key))
                this_logger.error("Key with the same hash: '{}'".format(hashmap[key_hash]))
                this_logger.error("Hash that collided: '{}'".format(hex(key_hash)))
                raise SystemExit(2, 'Hash collision detected.')
    print(hashmap)


def main(args):
    global parsed_args
    global program_parser
    global this_logger
    parsed = parse_args(args)
    if program_parser is None or parsed_args is None:
        raise SystemExit(1, 'Program argument parser not set or global argument set is None.')
    if parsed.verbose:
        for handler in this_logger.handlers:
            if isinstance(handler, type(logging.StreamHandler())):
                handler.setLevel(logging.DEBUG)
                this_logger.debug('Debug logging enabled')
    dump_db(parsed)


if __name__ == '__main__':
    main(sys.argv[1:])
