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
import os
import pathlib
import re
import sys

import deepmerge

bits = 8
buffer_size = 64 * 1024  # 64kib
collision_foresee = 1
default_db_filename = "properties.json"
default_header_filename = "JsonPropertyHashTable.h"
default_header_template = "template.h"
default_source_filename = "JsonPropertyHashTable.c"
default_source_template = "template.c.h"
flatten_separator = ":"
flatten_separator_api = "KS"
max_64bit = 0xFFFFFFFFFFFFFFFF
parsed_arguments = None
pattern_64bit = 0xA5A5A5A5A5A5A5A5
places_binary = bits
places_decimal = math.ceil(bits / math.log2(10))
places_hex = math.ceil(bits / 4)
places_octal = math.ceil(bits / 3)
precomputed_mask = 0x00
program_parser = None
internal_separator = "<->"
testing_property_key = "testing" + flatten_separator + "lookup"
testing_property_value = "working"
program_logger = logging.getLogger(__name__)


def parse_args(args):
    """
    Parse the program's arguments that control the program's behaviour.
    :param args: arguments to parse
    :return: the parsed arguments
    """
    parser = argparse.ArgumentParser(
            description="Creates a C source code file which contains a lookup table intended to be used as a "
                        "dictionary for properties in a JSON file. The program will calculate a hash which will be the "
                        "index of the array. The C program can calculate the hash and get the information from the "
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
                   "foresee to give up to the O(1) lookup time in order to keep the table as tidy as possible,")
    parser.add_argument('-j', '--json-file',
                        action='store',
                        default=default_db_filename,
                        help="This is the input file that will be used to generate the lookup table based on the JSON "
                             "properties of the objects in this file.",
                        type=argparse.FileType(bufsize=buffer_size))
    parser.add_argument('-a', '--header-template',
                        action='store',
                        default=default_header_template,
                        help="The template will be inserted at the beginning of the generated header.",
                        type=argparse.FileType(bufsize=buffer_size))
    parser.add_argument('-e', '--header',
                        action='store',
                        default=default_header_filename,
                        help="The output file where the header will be generated.",
                        type=argparse.FileType(mode='w', bufsize=buffer_size))
    parser.add_argument('-o', '--source-template',
                        action='store',
                        default=default_source_template,
                        help="The template will be inserted at the beginning of the generated source code.",
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
                        help="Define the linear lookup window to perform stores and lookups around the calculated hash "
                             "to resolve collisions.")
    parser.add_argument('-p', '--api-struct-name',
                        action='store', type=str, metavar='struct', default="jsonPropertyValue",
                        help="This is the name of the 'struct' that is exposed in the Header File (the API).")
    parser.add_argument('-i', '--api-table-name',
                        action='store', type=str, metavar='table', default="jsonPropertiesHashmap",
                        help="This is the name of the Hash Table (array) that is exposed in the Header File (the API).")
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help="Using this flag will print debug-level messages from the logger to the console.")
    global program_parser
    global parsed_arguments
    program_parser = parser
    parsed_arguments = parser.parse_args(args)
    return parsed_arguments


# Extracted from:
# https://towardsdatascience.com/how-to-flatten-deeply-nested-json-objects-in-non-recursive-elegant-python-55f96533103d
# Pretty funny that the implementation is recursive, tho.
def flatten_json(input_json_file: io.TextIOWrapper):
    """
    Flatten a JSON file into strings.
    :param input_json_file: the JSON file to flat
    :return: a dictionary which represents the flatten JSON
    """
    rex_current_working_directory = re.compile("<(?P<f>.*)>")
    rex_absolute_path = re.compile("{(?P<f>.*)}")
    output = {testing_property_key: testing_property_value}

    def include_recurse(json_path: str, file_list: [], main_dict: {}):
        """
        Perform a recursive inclusion of other JSON files, from an array inside the JSON files, called `include`, which
        can be either relative to the working directory (by using the '<' and '>' symbols around the file path),
        relative to the file which included it (by using the file path as is) or an absolute file path (by using the
        symbols '{' and '}' around the file path).
        Files are automatically "guarded", so they will be included only once. The first leaf file is included first,
        and the root file is included the latest. All properties that appear in more than one file will be overwrite
        to the latest definition (so it depends on which order the files are included, and that means that the root file
        will always override all previous definitions of a property or object).
        Generally speaking, you should call this function with an empty list and dictionary, unless you have a list of
        absolute paths to exclude or a dictionary which holds pre-defined values. Note that those pre-defined values
        will have the lowest precedence.
        :type json_path: str
        :type file_list: []
        :type main_dict: {}
        :param json_path: a string which is a path to a JSON file
        :param file_list: a list which holds all the absolute paths to file that should not be processed
        :param main_dict: a dictionary which holds the result of the processed files
        :return: the computed dictionary which is the combination of the files as described
        """
        with open(json_path) as open_json:
            json_dict = json.load(open_json)
            json_path_resolved = str(pathlib.Path(json_path).absolute())
            if json_path_resolved not in file_list:
                file_list.append(json_path_resolved)
            include_list = json_dict.pop("include", [])
            for include in include_list:
                include_matched = rex_current_working_directory.match(include)
                absolute_matched = rex_absolute_path.match(include)
                if include_matched is not None:
                    cwd = pathlib.Path(os.getcwd())
                    inferred_path = pathlib.Path(include_matched.group("f"))
                    path = str(cwd.joinpath(inferred_path).absolute())
                elif absolute_matched is not None:
                    inferred_path = pathlib.Path("/" + absolute_matched.group("f"))
                    path = str(inferred_path.absolute())
                else:
                    included_dir = pathlib.Path(json_path_resolved).parent
                    inferred_path = pathlib.Path(include)
                    path = str(included_dir.joinpath(inferred_path).absolute())
                if path not in file_list:  # Keep track of the included file to avoid recursion
                    file_list.append(path)
                    include_recurse(path, file_list, main_dict)
            deepmerge.always_merger.merge(main_dict, json_dict)
        return main_dict

    def flatten(python_dictionary: {}, prefix: str = ""):
        """
        The recursive call to flatten the JSON file, which accepts a Python dictionary as input and deeply flattens it.
        :type prefix: str
        :type python_dictionary: {}
        :param python_dictionary: a dictionary to add flatten JSON objects
        :param prefix: a prefix to prepend to properties
        """
        if type(python_dictionary) is dict:
            for value in python_dictionary:
                flatten(python_dictionary[value], prefix + value + flatten_separator)
        elif type(python_dictionary) is list:
            index = 0x00
            for value in python_dictionary:
                flatten(value, prefix + f"{index:d}" + flatten_separator)
                index += 0x01
        else:
            output[prefix[:-len(flatten_separator)]] = python_dictionary

    reduced_dictionary = include_recurse(input_json_file.name, [], {})
    flatten(reduced_dictionary)
    return output


def mask(value):
    """
    Mask a value to a given a number of relevant bits.
    :param value: the number to mask
    :return: the masked number
    """
    global precomputed_mask
    if precomputed_mask == 0x00:
        for i in range(bits):
            precomputed_mask = precomputed_mask << 0x01 | 0x01
    result = value & precomputed_mask
    return result


def truncate_mask(positions):
    """
    Generate a mask that can be used to truncate numbers to a certain number of relevant bits.
    :param positions: the bits that are relevant
    :return: a mask that if it's and-ed will truncate the number
    """
    truncate_masked = 0x00
    for i in range(positions):
        truncate_masked = truncate_masked << 0x01 | 0x01
    return truncate_masked


def rotate_left(value, positions):
    """
    Rotate a number to the left by considering only a number of relevant bits.
    :param value: the number to rotate
    :param positions: the number of relevant bits
    :return: the rotated and masked number
    """
    truncate_masked = truncate_mask(positions)
    left_shifted = value << positions
    left_shifted &= ~truncate_masked
    right_shifted = value >> (bits - positions)
    right_shifted = right_shifted & truncate_masked
    rotated = right_shifted | left_shifted
    return mask(rotated)


def rotate_right(value, positions):
    """
    Rotate a number to the right by considering only a number of relevant bits.
    :param value: the number to rotate
    :param positions: the number of relevant bits
    :return: the rotated and masked number
    """
    truncate_masked = truncate_mask(bits - positions)
    right_shifted = value >> positions
    right_shifted = right_shifted & truncate_masked
    left_shifted = value << (bits - positions)
    left_shifted &= ~truncate_masked
    rotated = right_shifted | left_shifted
    return mask(rotated)


def calculate_hash(string: str):
    """
    Calculate the `Hash1` hash of a given input string.
    :type string: str
    :param string: the string which is the input, it must have to be an ASCII string
    :return: the hash value for the input string
    """
    value = rotate_left(pattern_64bit, 0x01)
    ascii_bytes = string.encode("ASCII")
    for character in ascii_bytes:
        value = rotate_left(value, 0x01)
        value ^= ~character * ~len(string)
        value = rotate_left(value, 0x01)
        value ^= character * ~len(string)
    value = mask(value)
    return value


def calculate_hash2(string: str):
    """
    Calculate the `Hash2` hash of a given input string.
    :type string: str
    :param string: the string which is the input, it must have to be an ASCII string
    :return: the hash value for the input string
    """
    value = rotate_right(pattern_64bit, 0x01)
    ascii_bytes = string.encode("ASCII")
    for character in ascii_bytes:
        value = rotate_right(value, 0x01)
        value ^= character * ~len(string)
        value = rotate_right(value, 0x01)
        value ^= ~character * len(string)
    value = mask(value)
    return value


def hash_foresee(key_hash: int, name: str, hashmap: [], key: str, value: str):
    """
    Perform the linear lookup to resolve collisions around a given hash, inside the given hashmap.
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
    lowermost = 0x00 if (lowermost <= 0x00 or lowermost > mask(max_64bit)) else lowermost
    uppermost = key_hash + collision_foresee + 1
    uppermost = mask(max_64bit) if (uppermost < 0x00 or uppermost >= mask(max_64bit)) else uppermost
    for index in range(lowermost, uppermost, 1):
        if index == key_hash:
            continue
        program_logger.warning("--! Trying to solve collision by searching around the "
                               f"{name} hash {hex(key_hash)} in {hex(index)}...")
        in_map = hashmap[index]
        if in_map is None:
            hashmap[index] = f"{key}{internal_separator}{value}"
            collision_avoided = True
            program_logger.warning("--+ Collision avoided by moving hash "
                                   f"{hex(key_hash)} ({key_hash}) at index {index} ({hex(index)})")
            break
    return collision_avoided


def create_hashmap(args):
    """
    Create the hashmap inside a Python array.
    :param args: the program's parsed arguments
    :return: the generated hashmap
    """
    flatten_json_data = flatten_json(args.json_file)
    hashmap = [None] * (mask(max_64bit) + 1)
    for key, value in flatten_json_data.items():
        key_hash = calculate_hash(key)
        key_hash2 = calculate_hash2(key)
        program_logger.info(f"Key: {key}, Value: {value}, Hash1: {hex(key_hash)}, Hash2: {hex(key_hash2)}")
        in_map = hashmap[key_hash]
        if in_map is None:
            hashmap[key_hash] = f"{key}{internal_separator}{value}"
            program_logger.info("Key not found in the map, key added at index {key_hash}!")
        else:
            program_logger.warning(f"--! Collision detected, hash {hex(key_hash)} (key: '{key}') already in the table")
            collision_avoided = hash_foresee(key_hash, "Hash1", hashmap, key, value)
            if not collision_avoided:
                in_map = hashmap[key_hash2]
                program_logger.warning("--! Trying to use the alternative hash "
                                       f"{hex(key_hash2)} for hash {hex(key_hash)} (key: '{key}')")
                if in_map is None:
                    hashmap[key_hash2] = f"{key}{internal_separator}{value}"
                    collision_avoided = True
                    program_logger.warning(f"--+ Collision avoided by using the alternative hash {hex(key_hash2)}")
                else:
                    program_logger.warning(
                            f"--! Collision detected, hash {hex(key_hash2)} (key: '{key}') already in the table")
            if not collision_avoided:
                collision_avoided = hash_foresee(key_hash2, "Hash2", hashmap, key, value)
            if not collision_avoided:
                program_logger.warning(f"--- Failed to solve the collision for the second hash {hex(key_hash2)}")
                program_logger.error("--= Error: Unrecoverable collision detected...")
                program_logger.error(f"--= Length of the map: '{len(hashmap)}'")
                program_logger.error(f"--= Key to be inserted: '{key}'")
                program_logger.error(f"--= Key with the same hash: '{hashmap[key_hash].split(internal_separator)[0]}'")
                program_logger.error(f"--= Hash that collided: '{hex(key_hash)}'")
                raise SystemExit(2, "Unresolvable hash collision detected.")
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
        raise (3, "Can not proceed without template or output files.")
    with args.source_template as template:
        with io.StringIO("") as source:
            source.write("\n// Start of the array that holds the Hash Table as pointers to key-value structs...")
            source.write(f"\n\nconst struct {api_struct} *const {api_table}[{hashmap_length}] = {{")
            with io.StringIO("") as variables:
                nl = True
                variables.write("\n// Start of the structs that holds the key-value pairs...")
                for index in range(len(hashmap)):
                    hashable = hashmap[index]
                    ending = "," if index != hashmap_max_index else ""
                    if hashable is None:
                        source.write("{}NULL{}".format("\n\t" if nl else "", ending))
                        nl = False
                    else:
                        varname = f"{api_struct}{index:0{places_octal}o}"
                        key, val = hashable.split(internal_separator)
                        api_key = key.replace(flatten_separator, f"\"{flatten_separator_api}\"")
                        source.write(f"\n\t&{varname}{ending}")
                        variables.write(f"\n\n// ({index:0{places_binary}b}, {index:0{places_octal}o}, "
                                        f"{index:0{places_decimal}d}, {index:0{places_hex}x}) : "
                                        f"\"{key}\" -> \"{val}\"")
                        variables.write(f"\nstatic const struct {api_struct} {varname} = {{")
                        variables.write(f"\n\t\"{api_key}\",\n\t\"{val}\"\n}};")
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
            tpk = testing_property_key.replace(flatten_separator, f"\"{flatten_separator_api}\"")
            header.write("\n/// \\brief This is the separator used to separate the levels of properties.")
            header.write(f"\n#define {flatten_separator_api} \"{flatten_separator}\"")
            header.write("\n\n/// \\brief This is the testing property that will be used to test the lookup algorithm.")
            header.write(f"\nstatic const char *const {api_table}TestKey = \"{tpk}\";")
            header.write("\n\n/// \\brief This is the expected value for testing the lookup algorithm.")
            header.write(f"\nstatic const char *const {api_table}TestValue = \"{testing_property_value}\";")
            header.write("\n\n/// \\brief Represents a key-value pair, which is used to store inside the Hash Table.")
            header.write(f"\nstruct {api_struct} {{\n\tchar *key;\n\tchar *value;\n}};")
            header.write("\n\n/// \\brief The internal representation of the Hash Table.")
            header.write(f"\nconst struct {api_struct} *const {api_table}[{hashmap_length}];")
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
    global bits
    global collision_foresee
    global places_binary
    global places_decimal
    global places_hex
    global places_octal
    global program_logger
    rex = re.compile("(?P<n>\\d+)[Uu]?")
    bits = int(rex.match(parsed.bits).group("n"))
    collision_foresee = int(rex.match(parsed.foresee).group("n"))
    places_binary = bits
    places_decimal = math.ceil(bits / math.log2(10))
    places_hex = math.ceil(bits / 4)
    places_octal = math.ceil(bits / 3)
    if program_parser is None:
        raise SystemExit(11, 'Program argument parser not set or global argument set is None.')
    if parsed_arguments is None:
        raise SystemExit(14, 'Parsed program arguments is None.')
    if parsed.verbose:
        program_logger.setLevel(logging.DEBUG)
        program_logger.warning("Debug logging enabled")
    hashmap = create_hashmap(parsed)
    print_to_source(parsed, hashmap)


if __name__ == "__main__":
    main(sys.argv[1:])
