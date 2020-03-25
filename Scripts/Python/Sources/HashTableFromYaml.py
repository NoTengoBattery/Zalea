# ===-- HashTableFromYaml.py - Create a Hash Table for C from a Configuration YAML -------------------*- Python -*-=== #
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
# / property in a YAML object; and whose value is a pointer to the key-value pair of the property. This will implement a
# / kind of hash table, where the C program can calculate the hash of the property and use it as a index to the table
# / and get the pointer, and ultimately the value of the property's key.
# /
# ===--------------------------------------------------------------------------------------------------------------=== #

import argparse
import io
import logging
import os
import pathlib
import re
import sys

import dateutil.parser
import deepmerge
import math
import ruamel.yaml
import stringcase

bits = 8
buffer_size = 64 * 1024  # 64kib
collision_foresee = 1
default_db_filename = "properties.yaml"
default_header_filename = "YamlPropertyHashTable.h"
default_header_template = "template.h"
default_source_filename = "YamlPropertyHashTable.c"
default_source_template = "template.c.h"
flatten_separator = ":"
flatten_separator_api = "KS"
max_64bit = 0xFFFFFFFFFFFFFFFF
parsed_arguments = None
pattern_64bit = 0x8192A3B4C5D6E7F8 ^ 0x5A5A5A5A5A5A5A5A
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
yaml_parser = ruamel.yaml.YAML(typ="safe")


def parse_args(args: []):
    """
    Parse the program's arguments that control the program's behaviour.
    :param args: arguments to parse
    :return: the parsed arguments
    """
    parser = argparse.ArgumentParser(
            description="Creates a C source code file which contains a lookup table intended to be used as a "
                        "dictionary for properties in a YAML file. The program will calculate a hash which will be the "
                        "index of the array. The C program can calculate the hash and get the information from the "
                        "array, which is a pointer to the property string and it's value.",
            epilog="This file requires a template header, which will be included inside both the C header and the C "
                   "source file. The result will be in the provided output directory, which is one for the header and "
                   "other for the C source file. Those files can be included in a build system, just before this "
                   "program generates them.\n"
                   "To describe the generated lookup table more precisely: it have a Hopscotch open-addressing "
                   "collision solving algorithm. The linear lookup is defined by the foresee, and the hashes used to "
                   "compute values are custom hashing designed by the author. Those hashes support arbitrary length "
                   "output and are pretty well behaved for ASCII strings. The hash table is not resizable and no "
                   "re-hashing is made, and this makes the lookup O(1). However, it does require a power of 2 for the "
                   "storage of the index, and it does not scale well under load. However, users can overestimate the "
                   "foresee to give up to the O(1) lookup time in order to keep the table as tidy as possible,")
    parser.add_argument('-j', '--yaml-file',
                        action='store',
                        default=default_db_filename,
                        help="This is the input file that will be used to generate the lookup table based on the YAML "
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
                        action='store', type=str, metavar='struct', default="yamlPropertyValue",
                        help="This is the name of the 'struct' that is exposed in the Header File (the API).")
    parser.add_argument('-i', '--api-table-name',
                        action='store', type=str, metavar='table', default="yamlPropertiesHashmap",
                        help="This is the name of the Hash Table (array) that is exposed in the Header File (the API).")
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help="Using this flag will print debug-level messages from the logger to the console.")
    global program_parser
    global parsed_arguments
    program_parser = parser
    parsed_arguments = parser.parse_args(args)
    return parsed_arguments


# Include tags

def extract_path(string: str):
    """
    Extract the path of a string (for YAML include files).
    :param string: a string to be matched against the path format
    :return: a Path object with the computed path from the tag
    """
    rex_include_path = re.compile("<(?P<f>.*)>")
    path_matched = rex_include_path.match(string)
    if path_matched is not None:
        return pathlib.Path(path_matched.group("f"))
    raise SyntaxError("YAML files to be included must UNIX paths enclosed between the symbols < and >.")


@ruamel.yaml.yaml_object(yaml_parser)
class AbsoluteDirectory:
    """
    This class represent a path to include a file, relative to the root path directory.
    """
    yaml_tag = u'!$+'

    def __init__(self, value):
        self.path = extract_path(value)
        self.type = "abs"

    @classmethod
    def from_yaml(cls, constructor, node):
        return cls(node.value)


@ruamel.yaml.yaml_object(yaml_parser)
class RelativeDirectory:
    """
    This class represent a path to include a file, relative to the currently processed file.
    """
    yaml_tag = u'!$@'

    def __init__(self, value):
        self.path = extract_path(value)
        self.type = "rel"

    @classmethod
    def from_yaml(cls, constructor, node):
        return cls(node.value)


@ruamel.yaml.yaml_object(yaml_parser)
class WorkingDirectory:
    """
    This class represent a path to include a file, relative to the working directory.
    """
    yaml_tag = u'!$$'

    def __init__(self, value):
        self.path = extract_path(value)
        self.type = "cwd"

    @classmethod
    def from_yaml(cls, constructor, node):
        return cls(node.value)


# Data type tags

def hex_tag_format_signed(name: str, value: str):
    """
    Format the tags which accept a signed hexadecimal integer value to it's proper final format (to be inserted in the
    table).
    :param name: the name of the tag
    :param value: the value of the tag
    :return: a composite string with the value appended to the tag name
    """
    try:
        if re.match(r'^[+-]?0[Xx]', value):
            return f"{name}: {hex(int(value, 16))}"
        else:
            raise SyntaxError(f"The tag {name} only allows hexadecimal values that start with '[+-]0[xX]'.")
    except (SyntaxError, ValueError) as a:
        raise SyntaxError("Cannot parse a valid value for this tag.") from a


def hex_tag_format_unsigned(name: str, value: str):
    """
    Format the tags which accept an unsigned hexadecimal integer value to it's proper final format (to be inserted in
    the table).
    :param name: the name of the tag
    :param value: the value of the tag
    :return: a composite string with the value appended to the tag name
    """
    try:
        if re.match(r'^\+?0[Xx]', value):
            return f"{name}: {hex(int(value, 16))}"
        else:
            raise SyntaxError(f"The tag {name} only allows hexadecimal values that start with '\\+?0[xX]'.")
    except (SyntaxError, ValueError) as a:
        raise SyntaxError("Cannot parse a valid value for this tag.") from a


@ruamel.yaml.yaml_object(yaml_parser)
class ID:
    """
    This class represent an ID (unsigned).
    """
    yaml_tag = u'!id'

    def __init__(self, value):
        self.value = hex_tag_format_unsigned(self.yaml_tag, value)

    @classmethod
    def from_yaml(cls, constructor, node):
        return cls(node.value)


@ruamel.yaml.yaml_object(yaml_parser)
class Offset:
    """
    This class represent a C pointer difference (ptrdiff_t).
    """
    yaml_tag = u'!offset'

    def __init__(self, value):
        self.value = hex_tag_format_signed(self.yaml_tag, value)

    @classmethod
    def from_yaml(cls, constructor, node):
        return cls(node.value)


@ruamel.yaml.yaml_object(yaml_parser)
class Pointer:
    """
    This class represent a C pointer (uintptr_t).
    """
    yaml_tag = u'!pointer'

    def __init__(self, value):
        self.value = hex_tag_format_unsigned(self.yaml_tag, value)

    @classmethod
    def from_yaml(cls, constructor, node):
        return cls(node.value)


# Extracted from:
# https://towardsdatascience.com/how-to-flatten-deeply-nested-json-objects-in-non-recursive-elegant-python-55f96533103d
# Pretty funny that the implementation is recursive, tho.
def flatten_dictionary(input_properties_file: io.TextIOWrapper):
    """
    Flatten a YAML file into strings.
    :param input_properties_file: the YAML file to flat
    :return: a dictionary which represents the flatten YAML
    """
    result_dictionary = {}

    def include_recurse(properties_file_path: str, file_list: [], main_dict: {}):
        """
        Perform a recursive inclusion of other YAML files, from an array inside the YAML files, called `include`, which
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
        :type properties_file_path: str
        :type file_list: []
        :type main_dict: {}
        :param properties_file_path: a string which is a path to a YAML file
        :param file_list: a list which holds all the absolute paths to file that should not be processed
        :param main_dict: a dictionary which holds the result of the processed files
        :return: the computed dictionary which is the combination of the files as described
        """
        with open(properties_file_path) as open_yaml:
            properties_documents = yaml_parser.load_all(open_yaml)
            properties_file_path_resolved = str(pathlib.Path(properties_file_path).absolute())
            if properties_file_path_resolved not in file_list:
                file_list.append(properties_file_path_resolved)
            try:
                for properties_object in properties_documents:
                    try:
                        include_list = properties_object.pop("include", [])
                    except AttributeError:
                        raise SyntaxError("Empty documents are not allowed inside YAML files.")
                    for include in include_list:
                        try:
                            include_type = include.type
                            inferred_path = include.path
                        except AttributeError:
                            raise SyntaxError(
                                    "In order to include another YAML file, the path must have to be tagged with:\n\t"
                                    "$$: For paths relative to the working directory.\n\t$@: For paths relative to the "
                                    "including file.\n\t$+: For absolute paths.")
                        if include_type == "cwd":
                            working_directory = pathlib.Path(os.getcwd())
                            computed_path = str(working_directory.joinpath(inferred_path).absolute())
                        elif include_type == "abs":
                            computed_path = str(inferred_path.absolute())
                        elif include_type == "rel":
                            included_dir = pathlib.Path(properties_file_path_resolved).parent
                            computed_path = str(included_dir.joinpath(inferred_path).absolute())
                        else:
                            raise AssertionError("Cannot determine the type of file included.")
                        if computed_path not in file_list:  # Keep track of the included file to avoid recursion
                            file_list.append(computed_path)
                            include_recurse(computed_path, file_list, main_dict)
                    deepmerge.always_merger.merge(main_dict, properties_object)
            except SyntaxError as se:
                raise SyntaxError(f"Error while processing file '{properties_file_path_resolved}'") from se
        return main_dict

    def flatten(python_dictionary: {}, prefix: str = ""):
        """
        The recursive call to flatten the dictionary, which accepts a Python dictionary as input and deeply flattens it
        by converting it to a composed string with the properties separated by a string separator.
        :type prefix: str
        :type python_dictionary: {}
        :param python_dictionary: a dictionary to add the flatten objects
        :param prefix: a prefix to prepend to properties
        """
        if type(python_dictionary) is dict:
            for value in python_dictionary:
                flatten(python_dictionary[value], prefix + value + flatten_separator)
        elif type(python_dictionary) is list:
            index = 0x00
            for value in python_dictionary:
                flatten(value, prefix + f"{index:04d}" + flatten_separator)
                index += 0x01
        else:
            try:  # Try to unpack the custom tags :)
                python_dictionary = python_dictionary.value
            except AttributeError:
                pass
            result_dictionary[prefix[:-len(flatten_separator)]] = python_dictionary

    reduced_dictionary = include_recurse(input_properties_file.name, [], {})
    flatten(reduced_dictionary)
    result_dictionary[testing_property_key] = testing_property_value
    return result_dictionary


def mask(value: int):
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


def truncate_mask(positions: int):
    """
    Generate a mask that can be used to truncate numbers to a certain number of relevant bits.
    :param positions: the bits that are relevant
    :return: a mask that if it's and-ed will truncate the number
    """
    truncate_masked = 0x00
    for i in range(positions):
        truncate_masked = truncate_masked << 0x01 | 0x01
    return truncate_masked


def rotate_left(value: int, positions: int):
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


def rotate_right(value: int, positions: int):
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


def hash_foresee(key_hash: int, direction: int, name: str, hashmap: [], key: str, value: str):
    """
    Perform the linear lookup to resolve collisions around a given hash, inside the given hashmap.
    :type key_hash: int
    :type direction: int
    :type name: str
    :type hashmap: []
    :type key: str
    :type value: str
    :param key_hash: the calculated hash
    :param direction: the direction to lookup around first
    :param name: the name of the hash
    :param hashmap: the hashmap where the key and value will be inserted
    :param key: the key to the hashmap
    :param value: the value corresponding to the key
    :return: true if the collision was successfully avoided
    """
    collision_avoided = False
    lowermost = key_hash - collision_foresee
    lowermost = 0x00 if (lowermost <= 0x00 or lowermost > mask(max_64bit)) else lowermost
    uppermost = key_hash + collision_foresee
    uppermost = mask(max_64bit) if (uppermost < 0x00 or uppermost >= mask(max_64bit)) else uppermost
    direction = +0x01 if direction < 0x00 else -0x01
    start = lowermost if direction == +0x01 else uppermost
    end = (uppermost if direction == +0x01 else lowermost) + direction
    iter_range = range(start, end, direction)
    program_logger.debug(f"--- Searching range: {list(iter_range)}")
    for index in iter_range:
        if index == key_hash:
            continue
        program_logger.info("--! Trying to solve collision by searching around the "
                            f"{name} hash {hex(key_hash)} in {hex(index)}...")
        in_map = hashmap[index]
        if in_map is None:
            hashmap[index] = f"{key}{internal_separator}{value}"
            collision_avoided = True
            program_logger.warning("--+ Collision avoided by moving hash "
                                   f"{hex(key_hash)} ('{key}') at index {index} ({hex(index)})")
            break
    return collision_avoided


def create_hashmap(args: argparse.Namespace):
    """
    Create the hashmap inside a Python array.
    :param args: the program's parsed arguments
    :return: the generated hashmap
    """
    flatten_properties_data = flatten_dictionary(args.yaml_file)
    hashmap = [None] * (mask(max_64bit) + 1)
    for key, value in flatten_properties_data.items():
        key_hash = calculate_hash(key)
        key_hash2 = calculate_hash2(key)
        program_logger.info(f"Key: {key}, Value: {value}, Hash1: {hex(key_hash)}, Hash2: {hex(key_hash2)}")
        in_map = hashmap[key_hash]
        if in_map is None:
            hashmap[key_hash] = f"{key}{internal_separator}{value}"
            program_logger.info("Key not found in the map, key added at index {key_hash}!")
        else:
            program_logger.warning(f"--! Collision detected, hash {hex(key_hash)} (key: '{key}') is already in use")
            in_use = hashmap[key_hash].split(internal_separator)[0]
            program_logger.warning(f"--! Hash {hex(key_hash)} is currently used by the key '{in_use}'")
            collision_avoided = hash_foresee(key_hash, +1, "Hash1", hashmap, key, value)
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
                            f"--! Collision detected, hash {hex(key_hash2)} (key: '{key}') is already in use")
                    in_use = hashmap[key_hash2].split(internal_separator)[0]
                    program_logger.warning(f"--! Hash {hex(key_hash2)} is currently used by the key '{in_use}'")
            if not collision_avoided:
                collision_avoided = hash_foresee(key_hash2, -1, "Hash2", hashmap, key, value)
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
            source.write(f"\n\nconst struct {api_struct} {api_table}[{hashmap_length}] = {{")
            for index in range(len(hashmap)):
                comma = ' '
                if index != hashmap_max_index:
                    comma = ','
                hashable = hashmap[index]
                if hashable is None:
                    source.write(f"\n\t{{NULL, NULL}}{comma}")
                else:
                    key, val = hashable.split(internal_separator)
                    api_key = key.replace(flatten_separator, f"\" {flatten_separator_api} \"")
                    # Try to convert the value to a hexadecimal string if it's a integer.
                    try:
                        val = hex(int(val))
                    except ValueError:
                        pass
                    # Try to convert the value to a standard ISO 8601 date/time string
                    try:
                        val = "datetime: " + dateutil.parser.parse(val).isoformat()
                    except dateutil.parser.ParserError:
                        pass
                    source.write(f"\n\t{{\"{api_key}\", \"{val}\"}}{comma}")
                source.write(f"  // {index:0{places_decimal}d}, {index:0{places_hex}x}")
            source.write("\n};")
            source.write("\n")
            source.seek(0)
            with args.source as real_output:
                real_output.write(template.read())
                real_output.write(source.read())
    with args.header_template as template:
        with io.StringIO("") as header:
            tpk = testing_property_key.replace(flatten_separator, f"\" {flatten_separator_api} \"")
            header.write("\n/// \\brief This is the separator used to separate the levels of properties.")
            header.write(f"\n#define {flatten_separator_api} \"{flatten_separator}\"")
            header.write("\n\n/// \\brief This is the testing property that will be used to test the lookup algorithm.")
            header.write(f"\n#define {stringcase.constcase(api_table + 'TestKey')} \"{tpk}\"")
            header.write("\n\n/// \\brief This is the expected value for testing the lookup algorithm.")
            header.write(f"\n#define {stringcase.constcase(api_table + 'TestValue')} \"{testing_property_value}\"")
            header.write("\n\n/// \\brief Use this pattern to seed the hashing algorithm.")
            header.write(f"\n#define {stringcase.constcase(api_table + 'Pattern')} {hex(pattern_64bit)}")
            header.write("\n\n/// \\brief Represents a key-value pair, which is used to store inside the Hash Table.")
            header.write(f"\nstruct {api_struct} {{\n\tchar *key;\n\tchar *value;\n}};")
            header.write("\n\n/// \\brief The internal representation of the Hash Table.")
            header.write(f"\nconst struct {api_struct} {api_table}[{hashmap_length}];")
            header.write("\n")
            header.seek(0)
            with args.header as real_output:
                real_output.write(template.read())
                real_output.write(header.read())


def main(args: []):
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
