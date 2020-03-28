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

import deepmerge.strategy.core
import deepmerge.strategy.dict
import deepmerge.strategy.list
import math
import ruamel.yaml
import stringcase

bits: int = 8
buffer_size: int = 64 * 1024  # 64kib
collision_foresee: int = 1
default_db_filename: str = "properties.yaml"
default_header_filename: str = "YamlPropertyHashTable.h"
default_header_template: str = "template.h"
default_source_filename: str = "YamlPropertyHashTable.c"
default_source_template: str = "template.c.h"
flatten_separator: str = "\\x1f"
flatten_separator_api: str = "KS"
include_multiplicity: int = 3
max_64bit: int = 0xFFFFFFFFFFFFFFFF
parsed_arguments = None
pattern_64bit: int = 0x8192A3B4C5D6E7F8 ^ 0x5A5A5A5A5A5A5A5A
places_binary: int = bits
places_decimal: int = math.ceil(bits / math.log2(10))
places_hex: int = math.ceil(bits / 4)
places_octal: int = math.ceil(bits / 3)
precomputed_mask: int = 0x00
program_logger = logging.getLogger(__name__)
program_parser = None
rex_include_path = re.compile("<(?P<f>.*)>")
testing_property_key: str = "testing" + flatten_separator + "lookup"
testing_property_value: str = "working"
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
                   "other for the C source file. Those files can be included in a build system, just after this "
                   "program generates them.\n"
                   "To describe the generated lookup table more precisely: it have a Hopscotch open-addressing "
                   "collision solving algorithm. The linear lookup is defined by the foresee, and the hashes used to "
                   "compute values are custom hashing designed by the author. Those hashes support arbitrary length "
                   "output and are pretty well behaved for ASCII strings. The hash table is not resizable and no "
                   "re-hashing is made, and that makes the lookup O(1). However, it does require a power of 2 for the "
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


# Merging strategy: strategies to merge two objects. They define the overall merging logic of YAML objects.

# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class YamlListStrategies(deepmerge.strategy.list.ListStrategies):
    """
    Contains the strategies provided for lists.
    """
    NAME = "list"

    @staticmethod
    def strategy_yaml_sequence(config, path, base, other):
        """
        Try to perform a merge strategy. Note that the YAML specification does not support sequence merging. This is an
        add-on that does not follows the standard. This merger will try to replace values with the same key with the new
        ones, and will append non-existing values to the sequence.

        The merged sequence is guaranteed to be sorted. Note that this might mean that key indexes might change when new
        elements are added in a root file after the included files are processed. making node order flexible.

        :param config: the current merging configuration
        :param path: the path to the current merging conflict
        :param base: the base (original) value
        :param other: the other (to be merged) value

        :return: the merged list
        """

        def safe_return_key_lambda(x):
            try:
                return next(iter(x.items()))
            except AttributeError:
                return str(x), None

        safe_base = list(base)
        for b in base:
            for n in other:
                try:
                    for k, v in n.items():
                        if k in b:
                            b[k] = config.value_strategy(path + [k], b[k], v)
                        kv = {k: v}
                        if kv not in safe_base:
                            safe_base.append(kv)
                except AttributeError:
                    if n not in safe_base:
                        safe_base.append(n)
                except TypeError:
                    pass
        filtered_base = []
        [filtered_base.append(x) for x in safe_base if x not in filtered_base]
        filtered_base.sort(key=lambda x: safe_return_key_lambda(x))
        return filtered_base


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class YamlDictStrategies(deepmerge.strategy.dict.DictStrategies):
    """
    Contains the strategies provided for dictionaries.
    """
    NAME = "dict"

    @staticmethod
    def strategy_yaml_mapping(config, path, base, other):
        """
        Try to perform a merge strategy. Note that the YAML specification does not support mapping merging. This is an
        add-on that does not follows the standard. This merger will try to replace values with the same key with the new
        ones, and will append non-existing values to the sequence.

        This is the default merger strategy for dictionary from the deepmerge package.

        :param config: the current merging configuration
        :param path: the path to the current merging conflict
        :param base: the base (original) value
        :param other: the other (to be merged) value

        :return: the merged dictionary
        """
        for k, v in other.items():
            if k not in base:
                base[k] = v
            else:
                base[k] = config.value_strategy(path + [k], base[k], v)
        return base


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class YamlMerger(deepmerge.Merger):
    """
    :param type_strategies, List[Tuple]: a list of (Type, Strategy) pairs that should be used against incoming types.
    """
    PROVIDED_TYPE_STRATEGIES = {
        list: YamlListStrategies,
        dict: YamlDictStrategies
    }


merger = YamlMerger([(list, "yaml_sequence"), (dict, "yaml_mapping")], ["override"], [])


# Include tags: tags that represent a YAML file to be processed before the current file, then both results will merge

# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class YamlDirectory:
    """
    This class represent a path to include a file, the base class.
    """

    def __init__(self, value):
        self.path: str = value
        self.type: str = "???"

    def __str__(self):
        return self.path

    @classmethod
    def from_yaml(cls, constructor, node):
        def extract_path(string: str, mark) -> str:
            """
            Extract the path of a string (for YAML include files).

            :param string: a string to be matched against the path format
            :param mark: a mark to report the syntax error

            :return: a Path object with the computed path from the tag
            """
            path_matched = rex_include_path.match(string)
            if path_matched is not None:
                return pathlib.Path(path_matched.group("f"))
            raise SyntaxError("Included YAML files must have to be UNIX paths to YAML files, "
                              f"enclosed between '<' and '>',\n{mark}")

        return cls(extract_path(node.value, node.start_mark))


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
@ruamel.yaml.yaml_object(yaml_parser)
class AbsoluteDirectory(YamlDirectory):
    """
    This class represent a path to include a file, relative to the root path directory.
    """
    yaml_tag = u'!$+'

    def __init__(self, value):
        super().__init__(value)
        self.type: str = "abs"


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
@ruamel.yaml.yaml_object(yaml_parser)
class RelativeDirectory(YamlDirectory):
    """
    This class represent a path to include a file, relative to the currently processed file.
    """
    yaml_tag = u'!$@'

    def __init__(self, value):
        super().__init__(value)
        self.type: str = "rel"


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
@ruamel.yaml.yaml_object(yaml_parser)
class WorkingDirectory(YamlDirectory):
    """
    This class represent a path to include a file, relative to the working directory.
    """
    yaml_tag = u'!$$'

    def __init__(self, value):
        super().__init__(value)
        self.type: str = "cwd"


# Data type tags: all the strongly typed C-compatible data types. They correspond to one or more types and their value

# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class UnsignedCType:
    """
    This class represent an unsigned value. Unsigned values can be any value, but they must have to be unsigned
    integers. The children classes will restrict the type of the value.
    """

    def __init__(self, value):
        self.reduced_value: str = value

    def __str__(self):
        return self.reduced_value

    def __eq__(self, other):
        if isinstance(other, type(self)):
            return self.reduced_value == other.reduced_value
        return NotImplemented

    @classmethod
    def from_yaml(cls, constructor, node):
        def format_unsigned(tag: str, value: str, mark) -> str:
            """
            Format the tags which accept an unsigned integer value to it's proper final format ready to be inserted.

            :param tag: the name of the tag
            :param value: the value of the tag
            :param mark: a mark to report the syntax error

            :return: a composite string with the value appended to the tag name
            """
            string_format = "\\x01{}\\x02{}\\x03"
            try:
                if re.match(r'^\+?0[Xx]', value):
                    return string_format.format(tag, hex(int(value, 16)))
                if re.match(r'^\+?0[oO]?[0-7]', value):
                    return string_format.format(tag, hex(int(value, 8)))
                if re.match(r'^\+?0[bB]', value):
                    return string_format.format(tag, hex(int(value, 2)))
                if re.match(r'^\+?[\d]', value):
                    return string_format.format(tag, hex(int(value)))
                raise ValueError("Not a valid format for unsigned integers.")
            except (ValueError, TypeError) as e:
                raise SyntaxError(f"Can not parse a valid unsigned value for this tag,\n{mark}") from e

        return cls(format_unsigned(node.tag, node.value, node.start_mark))


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class SignedCType(UnsignedCType):
    """
    This class represent a signed value. Signed values can be any value, but they must have to be signed integers. The
    children classes will restrict the type of the value.
    """

    @classmethod
    def from_yaml(cls, constructor, node):
        def format_signed(tag: str, value: str, mark) -> str:
            """
            Format the tags which accept a signed integer value to it's proper final format ready to be inserted.

            :param tag: the name of the tag
            :param value: the value of the tag
            :param mark: a mark to report the syntax error

            :return: a composite string with the value appended to the tag name
            """
            string_format = "\\x01{}\\x02{}\\x03"
            try:
                if re.match(r'^[+-]?0[Xx]', value):
                    return string_format.format(tag, hex(int(value, 16)))
                if re.match(r'^[+-]?0[oO]?[0-7]', value):
                    return string_format.format(tag, hex(int(value, 8)))
                if re.match(r'^[+-]?0[bB]', value):
                    return string_format.format(tag, hex(int(value, 2)))
                if re.match(r'^[+-]?[\d]', value):
                    return string_format.format(tag, hex(int(value)))
                raise ValueError("Not a valid format for signed integers.")
            except (ValueError, TypeError) as e:
                raise SyntaxError(f"Can not parse a valid signed value for this tag,\n{mark}") from e

        return cls(format_signed(node.tag, node.value, node.start_mark))


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
@ruamel.yaml.yaml_object(yaml_parser)
class Offset(SignedCType):
    """
    This class represent a pointer difference, which also means an offset or an array index. They are always signed
    constants.

    They are always of the machine's `ptrdiff_t` size.
    """
    yaml_tag = u'!offset'


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
@ruamel.yaml.yaml_object(yaml_parser)
class Pointer(UnsignedCType):
    """
    This class represent a pointer. Pointers are always unsigned constants.

    They are always of the machine's `uintptr_t` size.
    """
    yaml_tag = u'!pointer'


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
@ruamel.yaml.yaml_object(yaml_parser)
class SID(SignedCType):
    """
    This class represent a signed ID. Signed ID's are always signed constants.

    They are always of the machine's `signed` size.
    """
    yaml_tag = u'!s-id'


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
@ruamel.yaml.yaml_object(yaml_parser)
class Signed(SignedCType):
    """
    This class represent a signed value. Signed value are always signed constants.

    They are always of the machine's `signed` size.
    """
    yaml_tag = u'!signed'


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
@ruamel.yaml.yaml_object(yaml_parser)
class UID(UnsignedCType):
    """
    This class represent an unsigned ID. Unsigned ID's are always unsigned constants.

    They are always of the machine's `unsigned` size.
    """
    yaml_tag = u'!u-id'


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
@ruamel.yaml.yaml_object(yaml_parser)
class Unsigned(UnsignedCType):
    """
    This class represent an unsigned value. Unsigned values are always unsigned constants.

    They are always of the machine's `unsigned` size.
    """
    yaml_tag = u'!unsigned'


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
@ruamel.yaml.yaml_object(yaml_parser)
class Range:
    """
    This class represent a range. Ranges can contain any value inside them, but they must have to define a start, an end
    and a name.
    """
    yaml_tag = u'!range'

    def __init__(self, value):
        self.reduced_value: str = value

    def __str__(self):
        return self.reduced_value

    @classmethod
    def from_yaml(cls, constructor, node):
        def reduce_range(tag: str, mapping_object: {}, mark) -> str:
            """
            Try to reduce the range tag to a string with a special format. Note that this is a generic range function,
            which can reduce any range (given the rules for range reduction).

            :param tag: the name of the tag
            :param mapping_object: the value of the tag
            :param mark: a mark to report the syntax error

            :return: a composite string with the value appended to the tag name
            """
            try:
                start = mapping_object['start']
                end = mapping_object['end']
                description = mapping_object['description']
                # Raise an error if description is not a string
                if type(description) is not str:
                    raise SyntaxError(f"The 'description' property must have to be a string,\n{mark}")
                # Attempt to give a proper format to string values
                try:
                    description.encode('ASCII')
                    if type(start) is str or type(end) is str:
                        # Try to parse the strings as ASCII-only strings
                        try:
                            start.encode('ASCII')
                            end.encode('ASCII')
                        except AttributeError:
                            pass
                except UnicodeEncodeError as e:
                    raise SyntaxError("All properties must have to be ASCII strings or any other valid tag or scalar,"
                                      f"\n{mark}") from e
                # Raise an error if start and end are not the same type
                if type(start) is not type(end):
                    raise SyntaxError(f"The 'start' and 'end' properties must have to be the same type,\n{mark}")
                return f"\\x01{tag}\\x02{start}\\x1e{end}\\x1e\\x02{description}\\x03\\x03"
            except KeyError as e:
                raise SyntaxError(
                        f"The {tag} tag requires 3 properties: 'start', 'end' and 'description'.\n{mark}") from e

        mapping = constructor.construct_mapping(node)
        return cls(reduce_range(node.tag, mapping, node.start_mark))


# Extracted from:
# https://towardsdatascience.com/how-to-flatten-deeply-nested-json-objects-in-non-recursive-elegant-python-55f96533103d
# Pretty funny that the implementation is recursive, tho.
def flatten_dictionary(input_properties_file: io.TextIOWrapper) -> {}:
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
            properties_path_absolute: str = str(pathlib.Path(properties_file_path).absolute())
            document_index: int = 0
            for properties_object in properties_documents:
                document_index += 1
                program_logger.debug(f"Processing document #{document_index} from file "
                                     f"'{properties_path_absolute}' for include directives...")
                file_and_document: str = f"{properties_path_absolute}:document:#{document_index}"
                per_document_includes: {} = file_carry.pop(file_and_document, {})
                try:
                    include_list: [] = properties_object.pop("$INCLUDE", [])
                except TypeError:
                    program_logger.debug("Could not find the include list, the file probably does not include files.")
                    include_list = []
                except AttributeError as e:
                    if properties_object is None:
                        raise SyntaxError(
                                "Empty documents are not allowed. Delete the document or insert a value.") from e
                    raise e
                try:
                    for include in include_list:
                        try:
                            inferred_path: pathlib.Path = include.path
                            include_type: str = include.type
                        except AttributeError:
                            continue
                        if include_type == "abs":
                            computed_path: str = str(inferred_path.absolute())
                            program_logger.debug(f"Found an include for an absolute path to '{computed_path}'")
                        elif include_type == "cwd":
                            working_directory = pathlib.Path(os.getcwd())
                            computed_path: str = str(working_directory.joinpath(inferred_path).absolute())
                            program_logger.debug(
                                    f"Found an include for a path relative to the c.w.d. to '{computed_path}'")
                        elif include_type == "rel":
                            included_dir = pathlib.Path(properties_path_absolute).parent
                            computed_path: str = str(included_dir.joinpath(inferred_path).absolute())
                            program_logger.debug(
                                    f"Found an include for a path relative to the file to '{computed_path}'")
                        else:
                            raise AssertionError("Can not determine the type of file included.")
                        warned_path: str = f"{computed_path}:warning"
                        warned_file: bool = per_document_includes.pop(warned_path, True)
                        program_logger.debug(f"Should warn about include multiplicity?: {warned_file}")
                        included_times: int = per_document_includes.pop(computed_path, 0) + 1
                        program_logger.debug(f"Total number of attempts to include: {included_times}")
                        per_document_includes[computed_path]: int = included_times
                        per_document_includes[warned_path]: bool = warned_file
                        file_carry[file_and_document] = per_document_includes
                        if included_times <= include_multiplicity:
                            program_logger.debug(f"Including file '{computed_path}' from '{file_and_document}'")
                            include_recurse(computed_path, file_carry, main_dict)
                            program_logger.info(f"Included file '{computed_path}' from '{file_and_document}'")
                        elif warned_file:
                            program_logger.warning(
                                    f"\n--! The file:\n"
                                    f"--~\t\t'{properties_path_absolute}',\n"
                                    f"--! Inside the document: #{document_index}; included the file:\n"
                                    f"--~\t\t'{computed_path}',\n"
                                    f"--! More than {include_multiplicity} times. "
                                    "Subsequent includes will be ignored.")
                            warned_file = False
                            program_logger.debug(f"Will warn about include multiplicity?: {warned_file}")
                            per_document_includes[warned_path] = warned_file
                        file_carry[file_and_document] = per_document_includes
                        program_logger.debug(f"File carry data structure: '{file_carry}'")
                except TypeError as e:
                    if include_list is None:
                        raise SyntaxError("Empty include lists are not allowed.") from e
                    raise e
                # Note that merging lists is not part of the YAML specification, so if a list is found in two files or
                # documents, the latest one found will replace the previous definitions.
                merger.merge(main_dict, properties_object)
                program_logger.info(f"Merged file '{properties_path_absolute}' into the return dictionary")
        return main_dict

    def flatten(python_dictionary: {}, prefix: str = ""):
        """
        The recursive call to flatten the dictionary, which accepts a Python dictionary as input and deeply flattens it
        by converting it to a composed string with the properties separated by a string separator.

        :param python_dictionary: a dictionary to add the flatten objects
        :param prefix: a prefix to prepend to properties

        :return: the flatten Python dictionary
        """
        if type(python_dictionary) is dict:
            for value in python_dictionary:
                flatten(python_dictionary[value], prefix + value + flatten_separator)
        elif type(python_dictionary) is list:
            records = {}
            for value in python_dictionary:
                if type(value) is dict:
                    computed_prefix = prefix + next(iter(value))
                else:
                    computed_prefix = prefix
                index = records.pop(computed_prefix, 0)
                flatten(value, prefix + f"0x{index:04x}" + flatten_separator)
                records[computed_prefix] = index + 1
        else:
            fixed_key = prefix[:-len(flatten_separator)]
            try:
                python_dictionary.encode('ASCII')
            except UnicodeEncodeError as e:
                raise UnicodeError(
                        f"Can not interpret '{fixed_key}' : '{python_dictionary}' as an ASCII string.") from e
            except AttributeError:
                pass
            result_dictionary[fixed_key] = python_dictionary

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
    truncate_masked: int = 0x00
    for i in range(positions):
        truncate_masked = truncate_masked << 0x01 | 0x01
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
    value: int = rotate_left(pattern_64bit, 0x01)
    ascii_bytes = string.encode("ASCII")
    for character in ascii_bytes:
        value = rotate_left(value, 0x01)
        value ^= ~character * ~len(string)
        value = rotate_left(value, 0x01)
        value ^= character * ~len(string)
    value = mask(value)
    return value


def calculate_hash2(string: str) -> int:
    """
    Calculate the `Hash2` hash of a given input string.

    :param string: the string which is the input, it must have to be an ASCII string

    :return: the hash value for the input string
    """
    value: int = rotate_right(pattern_64bit, 0x01)
    ascii_bytes = string.encode("ASCII")
    for character in ascii_bytes:
        value = rotate_right(value, 0x01)
        value ^= character * ~len(string)
        value = rotate_right(value, 0x01)
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
    lowermost = 0x00 if (lowermost <= 0x00 or lowermost > mask(max_64bit)) else lowermost
    uppermost: int = key_hash + collision_foresee
    uppermost = mask(max_64bit) if (uppermost < 0x00 or uppermost >= mask(max_64bit)) else uppermost
    direction: int = +0x01 if direction < 0x00 else -0x01
    start: int = lowermost if direction == +0x01 else uppermost
    end: int = (uppermost if direction == +0x01 else lowermost) + direction
    iter_range = range(start, end, direction)
    program_logger.debug(f"Linear searching range: {list(iter_range)}")
    for index in iter_range:
        if index == key_hash:
            continue
        program_logger.info(
                f"Trying to solve collision by searching around the {name} hash {hex(key_hash)} in {hex(index)}...")
        in_map: [] = hashmap[index]
        if in_map is None:
            hashmap[index] = (key, value)
            collision_avoided = True
            program_logger.warning(
                    f"--+ Collision avoided by moving hash {hex(key_hash)} ('{key}') at index {index} ({hex(index)})")
            break
    program_logger.info(f"Linear collision solving algorithm ended, resolved? {collision_avoided}")
    return collision_avoided


def create_hashmap(args: argparse.Namespace):
    """
    Create the hashmap inside a Python array.

    :param args: the program's parsed arguments

    :return: the generated hashmap
    """
    flatten_properties_data: {} = flatten_dictionary(args.yaml_file)
    hashmap: [] = [None] * (mask(max_64bit) + 1)
    for key, value in flatten_properties_data.items():
        key_hash: int = calculate_hash(key)
        key_hash2: int = calculate_hash2(key)
        program_logger.info(f"Key: \"{key}\", Value: \"{value}\", Hash1: {hex(key_hash)}, Hash2: {hex(key_hash2)}")
        in_map: () = hashmap[key_hash]
        if in_map is None:
            hashmap[key_hash] = (key, value)
            program_logger.info(f"Key not found in the map, key added at index {key_hash}!")
        else:
            program_logger.warning(f"--! Collision detected, hash {hex(key_hash)} (key: '{key}') is already in use")
            in_use: () = hashmap[key_hash]
            program_logger.warning(f"--! Hash {hex(key_hash)} is currently used by '{in_use}'")
            collision_avoided: bool = hash_foresee(key_hash, +1, "Hash1", hashmap, key, value)
            if not collision_avoided:
                in_map = hashmap[key_hash2]
                program_logger.warning("--! Trying to use the alternative hash "
                                       f"{hex(key_hash2)} for hash {hex(key_hash)} (key: '{key}')")
                if in_map is None:
                    hashmap[key_hash2] = (key, value)
                    collision_avoided = True
                    program_logger.warning(f"--+ Collision avoided by using the alternative hash {hex(key_hash2)}")
                else:
                    program_logger.warning(
                            f"--! Collision detected, hash {hex(key_hash2)} (key: '{key}') is already in use")
                    in_use: {} = hashmap[key_hash2]
                    program_logger.warning(f"--! Hash {hex(key_hash2)} is currently used by '{in_use}'")
            if not collision_avoided:
                collision_avoided = hash_foresee(key_hash2, -1, "Hash2", hashmap, key, value)
            if not collision_avoided:
                program_logger.warning(f"--- Failed to solve the collision for the second hash {hex(key_hash2)}")
                program_logger.error("--= Error: Unrecoverable collision detected...")
                program_logger.error(f"--= Length of the map: '{len(hashmap)}'")
                program_logger.error(f"--= Key to be inserted: '{key}'")
                program_logger.error(f"--= Key with the same hash: '{hashmap[key_hash]}'")
                program_logger.error(f"--= Hash that collided: '{hex(key_hash)}'")
                raise SystemExit(2, "Unresolvable hash collision detected.")
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
        raise (3, "Can not proceed without template or output files.")
    with args.source_template as template:
        with io.StringIO("") as source:
            source.write("\n// Start of the array that holds the Hash Table as pointers to key-value structs...")
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
                    # Try to unpack values that came from custom tags
                    try:
                        val = val.reduced_value
                        program_logger.info(f"Found packed value from tag for \"{key}\": {val}")
                    except AttributeError:
                        if type(val) is not str:
                            raise AssertionError("Due to the nature of the C language, all values must have to be "
                                                 "tagged with their correct type, including all integers. Only "
                                                 "strings, mappings and sequences can be untagged.\n\tOffending key "
                                                 f"path: '{key}'")
                        else:
                            val.encode('ASCII')
                    api_key = key.replace(flatten_separator, f"\" {flatten_separator_api} \"")
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
        logging.basicConfig(level=logging.DEBUG)
    hashmap = create_hashmap(parsed)
    print_to_source(parsed, hashmap)


if __name__ == "__main__":
    main(sys.argv[1:])
