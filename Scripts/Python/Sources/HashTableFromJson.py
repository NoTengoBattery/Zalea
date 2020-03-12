# ===-- HashTableFromJson.py - Create a Hash Table for C from a Configuration JSON -------------------*- Python -*-=== #
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
# / hash table, where the C program can calculate the hash of the property and use it as a index to the table and get
# / the pointer, and ultimately the value of the property.
# /
# ===--------------------------------------------------------------------------------------------------------------=== #

import argparse
import io
import json
import logging
import math
import re
import sys

bits = 8
buffer_size = 64 * 1024  # 64kb
collision_foresee = 1
default_db_filename = "properties.json"
default_header_filename = "JsonPropertyHashTable.h"
default_header_template = "template.h"
default_source_filename = "JsonPropertyHashTable.c"
default_source_template = "template.c.h"
max_64bit = 0xFFFFFFFFFFFFFFFF
parsed_args = None
pattern_64bit = 0xA5A5A5A5A5A5A5A5
places_binary = bits
places_decimal = math.ceil(bits / math.log2(10))
places_hex = math.ceil(bits / 4)
places_octal = math.ceil(bits / 3)
precomputed_mask = 0
program_parser = None
separator = "<->"
this_logger = logging.getLogger(__name__)


def parse_args(args):
    """
    Parse the program's arguments that control the program's behaviour.
    :param args: arguments to parse
    :return: the parsed arguments
    """
    parser = argparse.ArgumentParser(
            description="Creates a C source code file which contains a lookup table intended to be used as a "
                        "dictionary from properties in a JSON file. The program will calculate a hash which will be "
                        "the index of the array. The C program can calculate the hash and get the information from the "
                        "array, which is a pointer to the property string and it's value.",
            epilog="This file requires a template header, which will be included inside both the C header and the C "
                   "source file. The result will be in the provided output directory, which is one for the header and "
                   "other for the C source file. Those files can be included in a build system, just before this "
                   "program generates them. "
                   "To describe the generated lookup table more precisely: it have a Hopscotch open-addressing "
                   "collision solving algorithm. The linear lookup is defined by the foresee, and the hashes used to "
                   "compute values are custom hashing designed by the author. Those hashes support arbitrary length "
                   "output and are pretty well behaved for ASCII strings. The hash table is not resizable and no "
                   "re-hashing is made, and this makes the lookup O(1). However, it does require a power of 2 for the "
                   "storage of the index, and it does not scale well under load. However, users can overestimate the "
                   "foresee to give up the O(1) lookup time in order to keep the table as tidy as possible,")
    parser.add_argument('-j', '--json-file',
                        action='store',
                        default=default_db_filename,
                        help="This is the input file that will be used to generate the lookup table based on the JSON "
                             "properties of the objects in this file.",
                        type=argparse.FileType(bufsize=buffer_size))
    parser.add_argument('-a', '--header-template',
                        action='store',
                        default=default_header_template,
                        help="The template will be inserted at the beginning of the header code.",
                        type=argparse.FileType(bufsize=buffer_size))
    parser.add_argument('-e', '--header',
                        action='store',
                        default=default_header_filename,
                        help="The output file where the header will be generated.",
                        type=argparse.FileType(mode='w', bufsize=buffer_size))
    parser.add_argument('-o', '--source-template',
                        action='store',
                        default=default_source_template,
                        help="The template will be inserted at the beginning of the header code.",
                        type=argparse.FileType(bufsize=buffer_size))
    parser.add_argument('-s', '--source',
                        action='store',
                        default=default_source_filename,
                        help="The output file where the source code will be generated.",
                        type=argparse.FileType(mode='w', bufsize=buffer_size))
    parser.add_argument('-b', '--bits',
                        action='store', type=str, metavar='bits', default="8",
                        help="Define the number of bits that are used to generate the hash.")
    parser.add_argument('-f', '--foresee',
                        action='store', type=str, metavar='foresee', default="1",
                        help="Define the number of movements around the calculated index that the program will attempt"
                             "to seek around in order to find an available position. If the collision cannot be "
                             "recovered by this mean, using all of the available hashes, the program will exit with an "
                             "error code.")
    parser.add_argument('-p', '--api-struct-name',
                        action='store', type=str, metavar='struct', default="jsonPropertyValue",
                        help="This is the name of the 'struct' that is exposed in the Header File (the API).")
    parser.add_argument('-i', '--api-table-name',
                        action='store', type=str, metavar='table', default="jsonPropertiesHashmap",
                        help="This is the name of the 'Hash Table' that is exposed in the Header File (the API).")
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help="Using this flag will print debug messages from the logger to the console by default.")
    global program_parser
    program_parser = parser
    global parsed_args
    parsed_args = parser.parse_args(args)
    return parsed_args


# Extracted from:
# https://towardsdatascience.com/how-to-flatten-deeply-nested-json-objects-in-non-recursive-elegant-python-55f96533103d
def flatten_json(input_json):
    """
    Flatten a JSON file into strings.
    :param input_json: the JSON file to flat
    :return: a dictionary which represents the flatten JSON
    """
    output = {}

    def flatten(json_dict: {}, name=""):
        """
        The recursive call to flatten the JSON file.
        :type json_dict: {}
        :param json_dict: a dictionary to add flatten JSON objects
        :param name: a prefix to prepend to properties
        """
        if type(json_dict) is dict:
            for value in json_dict:
                flatten(json_dict[value], name + value + ":")
        elif type(json_dict) is list:
            index = 0
            for value in json_dict:
                flatten(value, name + str(index) + ":")
                index += 1
        else:
            output[name[:-1]] = json_dict

    flatten(input_json)
    return output


def mask(value):
    """
    Mask a number given a number of relevant bits.
    :param value: the number to mask
    :return: the masked number
    """
    global precomputed_mask
    if precomputed_mask == 0:
        precomputed_mask = 0
        for i in range(bits):
            precomputed_mask = precomputed_mask << 1 | 1
    result = value & precomputed_mask
    return result


def truncate_mask(positions):
    """
    Generate a mask that can be used to truncate numbers to a certain number of relevant bits.
    :param positions: the bits that are relevant
    :return: a mask that if it's and-ed will truncate the number
    """
    t_mask = 0
    for i in range(positions):
        t_mask = t_mask << 1 | 1
    return t_mask


def rotate_left(value, positions):
    """
    Rotate a number to the left by considering only a number of relevant bits.
    :param value: the number to rotate
    :param positions: the number of relevant bits
    :return: the rotated and masked number
    """
    tm = truncate_mask(positions)
    rle = value << positions
    rle &= ~tm
    rri = value >> (bits - positions)
    rri = rri & tm
    rotated = rri | rle
    return mask(rotated)


def rotate_right(value, positions):
    """
    Rotate a number to the right by considering only a number of relevant bits.
    :param value: the number to rotate
    :param positions: the number of relevant bits
    :return: the rotated and masked number
    """
    tm = truncate_mask(bits - positions)
    rri = value >> positions
    rri = rri & tm
    rle = value << (bits - positions)
    rle &= ~tm
    rotated = rri | rle
    return mask(rotated)


def calculate_hash(hash_input: str):
    """
    Calculate the `Hash1` hash of a given input string.
    :type hash_input: str
    :param hash_input: the string which is the input, it must have to be an ASCII string
    :return: the hash value for the input string
    """
    value = rotate_left(pattern_64bit, 1)
    ascii_bits = hash_input.encode("ASCII")
    for character in ascii_bits:
        value = rotate_left(value, 1)
        value ^= ~character * ~len(hash_input)
        value = rotate_left(value, 1)
        value ^= character * ~len(hash_input)
    value = mask(value)
    return value


def calculate_hash2(hash_input: str):
    """
    Calculate the `Hash2` hash of a given input string.
    :type hash_input: str
    :param hash_input: the string which is the input, it must have to be an ASCII string
    :return: the hash value for the input string
    """
    value = rotate_right(pattern_64bit, 1)
    ascii_bits = hash_input.encode("ASCII")
    for character in ascii_bits:
        value = rotate_right(value, 1)
        value ^= character * ~len(hash_input)
        value = rotate_right(value, 1)
        value ^= ~character * len(hash_input)
    value = mask(value)
    return value


def hash_foresee(key_hash: int, name: str, hashmap: [], key: str, value: str):
    """
    Perform the Linear Lookup to resolve collisions.
    :type key_hash: int
    :type name: str
    :type hashmap: []
    :type key: str
    :type value: str
    :param key_hash: the calculated hash
    :param name: the name of the hash
    :param hashmap: the hashmap where the key and value will be inserted
    :param key: the key to the hashmap
    :param value: the value corresponding to the key
    :return: true if the collision was successfully avoided
    """
    collision_avoided = False
    lowermost = key_hash - collision_foresee
    lowermost = 0 if (lowermost <= 0 or lowermost >= mask(max_64bit)) else lowermost
    uppermost = key_hash + collision_foresee + 1
    uppermost = mask(max_64bit) if (uppermost <= 0 or uppermost >= mask(max_64bit)) else uppermost
    step = 1 if uppermost >= lowermost else -1
    for index in range(lowermost, uppermost, step):
        if index == key_hash:
            continue
        this_logger.info("Trying to solve collision by searching around the {} hash {} in {}..."
                         .format(name, hex(key_hash), hex(index)))
        current_in_map = hashmap[index]
        if current_in_map is None:
            hashmap[index] = ("{}" + separator + "{}").format(key, value)
            collision_avoided = True
            this_logger.info("Collision avoided by moving hash {} ({}) at index {} ({})"
                             .format(hex(key_hash), key_hash, index, hex(index)))
            break
    return collision_avoided


def create_hashmap(args):
    """
    Create the hashmap inside a Python array.
    :param args: the program's parsed arguments
    :return: the generated hashmap
    """
    rex = re.compile("(?P<n>\\d+)[Uu]?")
    global bits
    bits = int(rex.match(args.bits).group("n"))
    global collision_foresee
    collision_foresee = int(rex.match(args.foresee).group("n"))
    global places_binary
    places_binary = bits
    global places_decimal
    places_decimal = math.ceil(bits / math.log2(10))
    global places_hex
    places_hex = math.ceil(bits / 4)
    global places_octal
    places_octal = math.ceil(bits / 3)
    json_data = json.load(args.json_file)
    json_data = flatten_json(json_data)
    hashmap = [None] * (mask(max_64bit) + 1)
    for key, value in json_data.items():
        key_hash = calculate_hash(key)
        this_logger.info("Key: {}, Value: {}, Current Hash: {}".format(key, value, hex(key_hash)))
        current_in_map = hashmap[key_hash]
        if current_in_map is None:
            hashmap[key_hash] = ("{}" + separator + "{}").format(key, value)
            this_logger.info("Key not found in the map, key added at index {}!".format(key_hash))
        else:
            collision_avoided = False
            key_hash2 = calculate_hash2(key)
            current_in_map = hashmap[key_hash2]
            this_logger.info("Trying to use the alternative hash {} for hash {} (key: {})"
                             .format(hex(key_hash2), hex(key_hash), key))
            if current_in_map is None:
                hashmap[key_hash2] = ("{}" + separator + "{}").format(key, value)
                collision_avoided = True
                this_logger.info("Collision avoided by using the alternative hash".format(key_hash))
            if not collision_avoided:
                collision_avoided = hash_foresee(key_hash, "Hash1", hashmap, key, value)
            if not collision_avoided:
                collision_avoided = hash_foresee(key_hash2, "Hash2", hashmap, key, value)
            if not collision_avoided:
                this_logger.warning("Failed to solve the collision for the second hash {}".format(hex(key_hash2)))
                this_logger.error("Error: Unrecoverable collision detected...".format(len(hashmap)))
                this_logger.error("Length of the map: '{}'".format(len(hashmap)))
                this_logger.error("Key to be inserted: '{}'".format(key))
                this_logger.error(
                        "Key with the same hash: '{}'".format(hashmap[key_hash].split("" + separator + "")[0]))
                this_logger.error("Hash that collided: '{}'".format(hex(key_hash)))
                raise SystemExit(2, "Hash collision detected.")
    return hashmap


def print_to_source(args, hashmap: []):
    """
    Print the computed hashmap to a C source code file.
    :type hashmap: []
    :param args: the program's arguments
    :param hashmap: the hashmap
    """
    api_struct = args.api_struct_name
    api_table = args.api_table_name
    hashmap_length = len(hashmap)
    hashmap_max_index = hashmap_length - 1
    if not args.header_template or not args.header or not args.source_template or not args.source:
        program_parser.error("Generating the source code requires the template and the output files.")
    with args.source_template as template:
        with io.StringIO("") as source:
            source.write("\nconst struct {} *const {}[{}] = {{"
                         .format(api_struct, api_table, hashmap_length))
            with io.StringIO("") as variables:
                nl = False
                for index in range(len(hashmap)):
                    hashable = hashmap[index]
                    ending = ", " if index != hashmap_max_index else ""
                    if hashable is None:
                        source.write("{}NULL{}".format("\n\t" if nl else "", ending))
                        nl = False
                    else:
                        varname = "{}{}".format(api_struct, f"{index:0{places_octal}o}")
                        key, val = hashable.split(separator)
                        source.write("\n\t&{}{}".format(varname, ending))
                        variables.write("\n// ({}, {}, {}, {}) : \"{}\" -> \"{}\""
                                        .format(f"{index:0{places_binary}b}",
                                                f"{index:0{places_octal}o}",
                                                f"{index:0{places_decimal}d}",
                                                f"{index:0{places_hex}x}",
                                                key, val))
                        variables.write("\nstatic const struct {} {} = {{\n\t".format(api_struct, varname))
                        variables.write("\n\"{}\",\n\t\"{}\"\n}};".format(key, val))
                        nl = True
                source.write("\n};")
                source.write("\n")
                source.seek(0)
                variables.write("\n")
                variables.seek(0)
                with args.source as real_output:
                    real_output.write(template.read())
                    real_output.write(variables.read())
                    real_output.write(source.read())
    with args.header_template as template:
        with io.StringIO("") as header:
            header.write("\n/// \\brief Represents a key-value pair, which is used to store inside the Hash Table.")
            header.write("\nstruct {} {{\n\tchar *key;\n\tchar *value;\n}};".format(api_struct))
            header.write("\n\n/// \\brief The internal representation of the Hash Table")
            header.write("\nconst struct {} *const {}[{}];".format(api_struct, api_table, hashmap_length))
            header.write("\n")
            header.seek(0)
            with args.header as real_output:
                real_output.write(template.read())
                real_output.write(header.read())


def main(args):
    """
    Main program (entry point).
    :param args: arguments from command line
    """
    parsed = parse_args(args)
    if program_parser is None or parsed_args is None:
        raise SystemExit(1, "Program argument parser not set or global argument set is None.")
    if parsed.verbose:
        this_logger.setLevel(logging.DEBUG)
        this_logger.warning("Debug logging enabled")
    hashmap = create_hashmap(parsed)
    print_to_source(parsed, hashmap)


if __name__ == "__main__":
    main(sys.argv[1:])
