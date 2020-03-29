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

import sys
from logging import DEBUG, basicConfig, getLogger
from math import ceil, log2

import base36
import re
# noinspection PyUnresolvedReferences
from YamlMerging import FileMarkedValue, GetYamlMerger
# noinspection PyUnresolvedReferences
from YamlTags import FileMarker, InitializeIncludeTags, InitializeStronglyTypedTags, StringCType, YamlInclude
from argparse import ArgumentParser, FileType, Namespace
from io import StringIO, TextIOWrapper
from os import getcwd
from pathlib import Path
from ruamel.yaml import YAML
from stringcase import constcase

bits: int = 8
buffer_size: int = 64 * 1024  # 64kib
collision_foresee: int = 1
default_db_filename: str = "properties.yaml"
default_header_filename: str = "YamlPropertyHashTable.h"
default_header_template: str = "template.h"
default_source_filename: str = "YamlPropertyHashTable.c"
default_source_template: str = "template.c.h"
flatten_separator: str = "\\x1f"
flatten_separator_api: str = "PS"
include_multiplicity: int = 1
max_64bit: int = 0xFFFFFFFFFFFFFFFF
parsed_arguments = None
pattern_64bit: int = 0x8192A3B4C5D6E7F8 ^ 0x5A5A5A5A5A5A5A5A
places_binary: int = bits
places_decimal: int = ceil(bits / log2(10))
places_hex: int = ceil(bits / 4)
places_octal: int = ceil(bits / 3)
precomputed_mask: int = 0
print_separator: str = "::"
program_logger = getLogger(__name__)
program_parser = None
testing_property_key: str = "testing" + flatten_separator + "lookup"
testing_property_value: StringCType = StringCType("working")
yaml_merger = GetYamlMerger()
yaml_parser = YAML(typ="safe")

InitializeIncludeTags(yaml_parser)
InitializeStronglyTypedTags(yaml_parser)


def parse_args(args: []):
    """
    Parse the program's arguments that control the program's behaviour.

    :param args: arguments to parse

    :return: the parsed arguments
    """
    parser = ArgumentParser(
            description="Creates a C source code file which contains a lookup table intended to be used as a "
                        "dictionary for properties in a YAML file. The program will calculate a hash which will be the "
                        "index of the array. The C program can calculate the hash and get the information from the "
                        "array, which is a pointer to the property string and it's value.",
            epilog="This file requires a template header, which will be included inside both the C header and the C "
                   "source file. The result will be in the provided output directory, which is one for the header and "
                   "other for the C source file. Those files can be included in a build system, just after this "
                   "program generates them.\n"
                   "To describe the generated lookup table more precisely: it have a Hopscotch open-addressing "
                   "collision solving algorithm. The linear lookup is defined by the foresee, and the hashes used to "
                   "compute values are custom hashing designed by the author. Those hashes support arbitrary length "
                   "output and are pretty well behaved for ASCII strings. The hash table is not resizable and no "
                   "re-hashing is made, and that makes the lookup O(1). However, it does require a power of 2 for the "
                   "storage of the index, and it does not scale well under load. Anyway, users can overestimate the "
                   "foresee to give up to the O(1) lookup time in order to keep the table as tidy as possible.")
    parser.add_argument('-y', '--yaml-file',
                        action='store',
                        default=default_db_filename,
                        help="This is the input file that will be used to generate the lookup table based on the YAML "
                             "properties of the objects in this file.",
                        type=FileType(bufsize=buffer_size))
    parser.add_argument('-e', '--header-template',
                        action='store',
                        default=default_header_template,
                        help="The template will be inserted at the beginning of the generated header.",
                        type=FileType(bufsize=buffer_size))
    parser.add_argument('-a', '--header',
                        action='store',
                        default=default_header_filename,
                        help="The output file where the header will be generated.",
                        type=FileType(mode='w', bufsize=buffer_size))
    parser.add_argument('-s', '--source-template',
                        action='store',
                        default=default_source_template,
                        help="The template will be inserted at the beginning of the generated source code.",
                        type=FileType(bufsize=buffer_size))
    parser.add_argument('-o', '--source',
                        action='store',
                        default=default_source_filename,
                        help="The output file where the source code will be generated.",
                        type=FileType(mode='w', bufsize=buffer_size))
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


def flatten_dictionary(input_properties_file: TextIOWrapper) -> {}:
    """
    Flatten a YAML file into strings.

    :param input_properties_file: the YAML file to flat

    :return: a dictionary which represents the flatten YAML
    """
    result_dictionary = {}

    def include_recurse(properties_file_path: str, file_carry: {}, main_dict: {}) -> {}:
        """
        Perform a recursive inclusion of other YAML files, from tags inside the YAML files, which can be either
        relative to the working directory (by using the '$$' tag), relative to the file which included it (by using
        the '$@' tag) or an absolute file path (by using the '$+' tag). Files are automatically "guarded",
        so they will be included only once per file. The first leaf file is included first, and the root file is
        included the latest. All properties that appear in more than one file or document will be replaced to the
        latest definition (so it depends on which order the files are included, and that means that the latest
        document of the root file will always override all previous definitions of a property or object).

        Generally speaking, you should call this function with two empty dictionaries, unless you have a list of
        absolute paths to exclude or a dictionary which holds pre-defined values. Note that those pre-defined values
        will have the lowest precedence.

        :param properties_file_path: a string which is a path to a YAML file
        :param file_carry: a dictionary which holds all the absolute paths to file that should not be processed
        :param main_dict: a dictionary which holds the result of the processed files

        :return: the computed dictionary which is the combination of the files as described
        """
        with open(properties_file_path) as open_yaml:
            properties_documents = yaml_parser.load_all(open_yaml)
            properties_path_absolute: str = str(Path(properties_file_path).absolute())
            document_index: int = 0
            for properties_object in properties_documents:
                document_index += 1
                program_logger.debug(f"Processing document #{document_index} from file "
                                     f"'{properties_path_absolute}' for include directives...")
                file_and_document: str = f"{properties_path_absolute}#{document_index}"
                per_document_includes: {} = file_carry.pop(file_and_document, {})
                try:
                    include_list: [] = properties_object.pop(StringCType("$INCLUDE"), [])
                except TypeError:
                    program_logger.debug("Could not find the include list, the file probably does not include files.")
                    include_list = []
                except AttributeError as e:
                    if properties_object is None:
                        raise AssertionError("Empty documents are not allowed.") from e
                    raise e
                try:
                    for include in include_list:
                        try:
                            inferred_path: Path = include.path
                            include_type: str = include.type
                        except AttributeError:
                            if type(include) is not FileMarker:
                                raise AssertionError("Invalid tag inside the include sequence.")
                            continue
                        if include_type == "abs":
                            computed_path: str = str(inferred_path.absolute())
                            program_logger.debug(f"Found an absolute path include: '{computed_path}'")
                        elif include_type == "cwd":
                            working_directory = Path(getcwd())
                            computed_path: str = str(working_directory.joinpath(inferred_path).absolute())
                            program_logger.debug(f"Found an include relative to working directory: '{computed_path}'")
                        elif include_type == "rel":
                            included_dir = Path(properties_path_absolute).parent
                            computed_path: str = str(included_dir.joinpath(inferred_path).absolute())
                            program_logger.debug(f"Found an include relative to the current file: '{computed_path}'")
                        else:
                            raise AssertionError("Can not determine the type of file included.")
                        warned_path: str = f"{computed_path}:warning"
                        warned_file: bool = per_document_includes.pop(warned_path, True)
                        included_times: int = per_document_includes.pop(computed_path, 0) + 1
                        per_document_includes[computed_path]: int = included_times
                        per_document_includes[warned_path]: bool = warned_file
                        file_carry[file_and_document] = per_document_includes
                        if included_times <= include_multiplicity:
                            program_logger.debug(f"Total number of attempts to include: {included_times}")
                            program_logger.debug(f"Including file '{computed_path}' from '{file_and_document}'...")
                            include_recurse(computed_path, file_carry, main_dict)
                            program_logger.info(f"Included file '{computed_path}' from '{file_and_document}'")
                        elif warned_file:
                            program_logger.warning(
                                    f"--! The file:\n"
                                    f"--~\t\t'{properties_path_absolute}',\n"
                                    f"--! Inside the document: #{document_index}; included the file:\n"
                                    f"--~\t\t'{computed_path}',\n"
                                    f"--! More than {include_multiplicity} times. "
                                    "Subsequent includes will be ignored.")
                            program_logger.debug(f"Will warn about include multiplicity?: {warned_file}")
                            per_document_includes[warned_path] = False
                        file_carry[file_and_document] = per_document_includes
                except TypeError as e:
                    if include_list is None:
                        raise AssertionError("Empty include lists are not allowed.") from e
                    raise e
                # Note that merging lists is not part of the YAML specification, so if a list is found in two files or
                # documents, the latest one found will replace the previous definitions.
                yaml_merger.merge(main_dict, properties_object)
                program_logger.info(f"Merged file '{properties_path_absolute}#{document_index}' into the dictionary")
        yaml_merger.merge(main_dict, main_dict)
        return main_dict

    def flatten(reducible: {}, prefix: str = ""):
        """
        The recursive call to flatten the dictionary, which accepts a Python dictionary as input and deeply flattens it
        by converting it to a composed string with the properties separated by a string separator.

        :param reducible: a dictionary to add the flatten objects
        :param prefix: a prefix to prepend to properties

        :return: the flatten Python dictionary
        """
        fixed_key: str = prefix[:-len(flatten_separator)]
        printable_fixed_key: str = fixed_key.replace(flatten_separator, print_separator)
        if type(reducible) is dict:
            for key in reducible:
                flatten(reducible[key], f"{prefix}{key}{flatten_separator}")
        elif type(reducible) is list:
            indexes_mapping: {} = {}
            for value in reducible:
                if type(value) is FileMarkedValue:
                    value = value.contents
                else:
                    if isinstance(value, YamlInclude):
                        raise AssertionError("Found include tags outside of the include sequence.\n\t"
                                             f"Offending key path: '{printable_fixed_key}'")
                    raise AssertionError("Internal: Invalid encapsulation inside the YAML sequence.")
                if type(value) is dict:
                    computed_prefix = f"{prefix}{flatten_separator}{next(iter(value))}"
                else:
                    computed_prefix = prefix
                index = indexes_mapping.pop(computed_prefix, 0)
                max_36 = "3W5E11264SGSF"
                max_index = int(max_36, 36)
                if index > max_index:
                    raise AssertionError(f"Maximum elements for table reached: {max_index + 1}.\n\t"
                                         f"Offending key path: '{printable_fixed_key}'")
                printable_index = base36.dumps(index).upper().rjust(len(max_36), "0")
                flatten(value, prefix + printable_index + flatten_separator)
                indexes_mapping[computed_prefix] = index + 1
        else:
            try:
                fixed_key.encode("ASCII")
            except UnicodeEncodeError as e:
                raise AssertionError(f"Can not interpret key '{printable_fixed_key}' as an ASCII string.") from e
            if fixed_key not in result_dictionary:
                result_dictionary[fixed_key] = reducible
            else:
                raise AssertionError("Unrecoverable collision detected while building the key-value mappings.\n\t"
                                     f"Offending key path: '{printable_fixed_key}'.")

    reduced_dictionary = include_recurse(input_properties_file.name, {}, {})
    flatten(reduced_dictionary)
    result_dictionary[testing_property_key] = testing_property_value
    return result_dictionary


def mask(value: int) -> int:
    """
    Mask a value to a given a number of relevant bits.

    :param value: the number to mask

    :return: the masked number
    """
    global precomputed_mask
    if precomputed_mask == 0:
        program_logger.debug("Computing the mask...")
        for i in range(bits):
            precomputed_mask = precomputed_mask << 1 | 1
    result: int = value & precomputed_mask
    return result


def truncate_mask(positions: int) -> int:
    """
    Generate a mask that can be used to truncate numbers to a certain number of relevant bits.

    :param positions: the bits that are relevant

    :return: a mask that if it's and-ed will truncate the number
    """
    truncate_masked: int = 0
    for i in range(positions):
        truncate_masked = truncate_masked << 1 | 1
    return truncate_masked


def rotate_left(value: int, positions: int) -> int:
    """
    Rotate a number to the left by considering only a number of relevant bits.

    :param value: the number to rotate
    :param positions: the number of relevant bits

    :return: the rotated and masked number
    """
    truncate_masked: int = truncate_mask(positions)
    left_shifted: int = value << positions
    left_shifted &= ~truncate_masked
    right_shifted: int = value >> (bits - positions)
    right_shifted &= truncate_masked
    rotated: int = right_shifted | left_shifted
    rotated = mask(rotated)
    return rotated


def rotate_right(value: int, positions: int) -> int:
    """
    Rotate a number to the right by considering only a number of relevant bits.

    :param value: the number to rotate
    :param positions: the number of relevant bits

    :return: the rotated and masked number
    """
    truncate_masked: int = truncate_mask(bits - positions)
    right_shifted: int = value >> positions
    right_shifted &= truncate_masked
    left_shifted: int = value << (bits - positions)
    left_shifted &= ~truncate_masked
    rotated: int = right_shifted | left_shifted
    rotated = mask(rotated)
    return rotated


def calculate_hash(string: str) -> int:
    """
    Calculate the `Hash1` hash of a given input string.

    :param string: the string which is the input, it must have to be an ASCII string

    :return: the hash value for the input string
    """
    value: int = rotate_left(pattern_64bit, 1)
    ascii_bytes = string.encode("ASCII")
    for character in ascii_bytes:
        value = rotate_left(value, 1)
        value ^= ~character * ~len(string)
        value = rotate_left(value, 1)
        value ^= character * ~len(string)
    value = mask(value)
    return value


def calculate_hash2(string: str) -> int:
    """
    Calculate the `Hash2` hash of a given input string.

    :param string: the string which is the input, it must have to be an ASCII string

    :return: the hash value for the input string
    """
    value: int = rotate_right(pattern_64bit, 1)
    ascii_bytes = string.encode("ASCII")
    for character in ascii_bytes:
        value = rotate_right(value, 1)
        value ^= character * ~len(string)
        value = rotate_right(value, 1)
        value ^= ~character * len(string)
    value = mask(value)
    return value


def hash_foresee(key_hash: int, direction: int, name: str, hashmap: [], key: str, value: str) -> bool:
    """
    Perform the linear lookup to resolve collisions around a given hash, inside the given hashmap.

    :param key_hash: the calculated hash
    :param direction: the direction to lookup around first
    :param name: the name of the hash
    :param hashmap: the hashmap where the key and value will be inserted
    :param key: the key to the hashmap
    :param value: the value corresponding to the key

    :return: true if the collision was successfully avoided
    """
    program_logger.info("Linear collision solving algorithm started...")
    collision_avoided: bool = False
    lowermost: int = key_hash - collision_foresee
    lowermost = 0 if (lowermost <= 0 or lowermost > mask(max_64bit)) else lowermost
    uppermost: int = key_hash + collision_foresee
    uppermost = mask(max_64bit) if (uppermost < 0 or uppermost >= mask(max_64bit)) else uppermost
    direction: int = 1 if direction < 0 else -1
    start: int = lowermost if direction == 1 else uppermost
    end: int = (uppermost if direction == 1 else lowermost) + direction
    iter_range = range(start, end, direction)
    program_logger.debug(f"Linear searching range: {list(iter_range)}")
    index: int
    for index in iter_range:
        if index == key_hash:
            continue
        program_logger.info(
                f"Trying to solve collision by searching around the {name} index {hex(key_hash)} in {hex(index)}...")
        in_map: [] = hashmap[index]
        if in_map is None:
            hashmap[index] = (key, value)
            collision_avoided = True
            printable_key: str = key.replace(flatten_separator, print_separator)
            program_logger.warning(f"--+ Collision solved by moving the hash {hex(key_hash)} ('{printable_key}'),\n\t\t"
                                   f"at index {index} ({hex(index)})")
            break
    program_logger.info(f"Linear collision solving algorithm ended, resolved? {collision_avoided}")
    return collision_avoided


def create_hashmap(args: Namespace):
    """
    Create the hashmap inside a Python array.

    :param args: the program's parsed arguments

    :return: the generated hashmap
    """
    flatten_properties: {} = flatten_dictionary(args.yaml_file)
    hashmap: [] = [None] * (mask(max_64bit) + 1)
    for key, value in flatten_properties.items():
        printable_key: str = key.replace(flatten_separator, print_separator)
        key_hash: int = calculate_hash(key)
        key_hash2: int = calculate_hash2(key)
        program_logger.info(f"Key: \"{printable_key}\", Value: \"{value}\"")
        program_logger.info(f"Hash1: {hex(key_hash)}, Hash2: {hex(key_hash2)}")
        in_map: () = hashmap[key_hash]
        if in_map is None:
            hashmap[key_hash] = (key, value)
            program_logger.info(f"Key not found in the map, key added at index {key_hash}!")
            continue
        else:
            in_use: () = hashmap[key_hash]
            program_logger.warning(f"--! Index {hex(key_hash)} is currently used by '{in_use}'"
                                   .replace("\\" + flatten_separator, print_separator))
            collision_avoided = hash_foresee(key_hash, 1, "Hash1", hashmap, key, value)
        if collision_avoided:
            continue
        else:
            in_map = hashmap[key_hash2]
            program_logger.warning(f"--! Trying to use the alternative hash {hex(key_hash2)},\n\t"
                                   f"for hash{hex(key_hash)} (key: '{printable_key}')")
            if in_map is None:
                hashmap[key_hash2] = (key, value)
                program_logger.warning(f"--+ Collision resolved by using the alternative hash {hex(key_hash2)}")
                continue
            else:
                in_use: {} = hashmap[key_hash2]
                program_logger.warning(f"--! Index {hex(key_hash2)} is currently used by '{in_use}'".
                                       replace("\\" + flatten_separator, print_separator))
                collision_avoided = False
        if collision_avoided:
            continue
        else:
            collision_avoided = hash_foresee(key_hash2, -1, "Hash2", hashmap, key, value)
        if collision_avoided:
            continue
        else:
            program_logger.warning("--- Failed to solve the collision!")
            program_logger.error("--= Error: Unrecoverable collision detected...")
            program_logger.error(f"--= Length of the map: '{len(hashmap)}'")
            program_logger.error(f"--= Key to be inserted: '{printable_key}'")
            program_logger.error(f"--= Key with the same hash: '{hashmap[key_hash]}'")
            program_logger.error(f"--= Index that collided: '{hex(key_hash)}'")
            program_logger.error(f"--= Alternative index that collided: '{hex(key_hash2)}'")
            raise SystemExit("Unresolvable hash collision detected.")
    return hashmap


def print_to_source(args, hashmap: []):
    """
    Print the computed hashmap to a C source code file.

    :param args: the program's arguments
    :param hashmap: the hashmap
    """
    api_struct = args.api_struct_name
    api_table = args.api_table_name
    hashmap_length = len(hashmap)
    hashmap_max_index = hashmap_length - 1
    if not args.header_template or not args.header or not args.source_template or not args.source:
        program_parser.error("Generating the source code requires the template and the output files.")
        raise SystemExit("Can not proceed without template or output files.")
    with args.source_template as template:
        with StringIO("") as source:
            source.write("\n// Start of the array that holds the Hash Table of the key-value pairs...")
            source.write(f"\n\nconst struct {api_struct} {api_table}[{hashmap_length}] = {{")
            for index in range(len(hashmap)):
                comma = ' '
                if index != hashmap_max_index:
                    comma = ','
                value_at_index = hashmap[index]
                if value_at_index is None:
                    source.write(f"\n\t{{NULL, NULL}}{comma}")
                else:
                    key, val = value_at_index
                    printable_key: str = key.replace(flatten_separator, print_separator)
                    try:
                        val = val.reduced_value
                        program_logger.info(f"Found packed value from tag for \"{printable_key}\": {val}")
                    except AttributeError:
                        raise AssertionError(
                                "Due to the nature of the C language, all values must have to be tagged with their "
                                "correct type, including all integers. Only strings, mappings and sequences can be "
                                "untagged.\n\t"
                                f"Offending key path: '{printable_key}'")
                    api_key = key.replace(flatten_separator, f"\"{flatten_separator_api}\"")
                    source.write(f"\n\t{{\"{api_key}\",\n\t \"{val}\"}}{comma}")
                source.write(f"  // {index:0{places_decimal}d}, {index:0{places_hex}x}")
            source.write("\n};")
            source.write("\n")
            source.seek(0)
            with args.source as real_output:
                real_output.write(template.read())
                real_output.write(source.read())
    with args.header_template as template:
        with StringIO("") as header:
            tpk = testing_property_key.replace(flatten_separator, f"\"{flatten_separator_api}\"")
            header.write("\n/// \\brief This is the separator used to separate the levels of properties.")
            header.write(f"\n#define {flatten_separator_api} \"{flatten_separator}\"")
            header.write("\n\n/// \\brief This is the testing property that will be used to test the lookup algorithm.")
            header.write(f"\n#define {constcase(api_table + 'TestKey')} \"{tpk}\"")
            header.write("\n\n/// \\brief This is the expected value for testing the lookup algorithm.")
            header.write(f"\n#define {constcase(api_table + 'TestValue')} \"{testing_property_value}\"")
            header.write("\n\n/// \\brief Use this pattern to seed the hashing algorithm.")
            header.write(f"\n#define {constcase(api_table + 'Pattern')} {hex(pattern_64bit)}")
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
    places_decimal = ceil(bits / log2(10))
    places_hex = ceil(bits / 4)
    places_octal = ceil(bits / 3)
    if program_parser is None:
        raise SystemExit("Program argument parser not set or global argument set is None.")
    if parsed_arguments is None:
        raise SystemExit("Parsed program arguments is None.")
    if parsed.verbose:
        basicConfig(level=DEBUG)
    hashmap = create_hashmap(parsed)
    print_to_source(parsed, hashmap)


if __name__ == "__main__":
    main(sys.argv[1:])
