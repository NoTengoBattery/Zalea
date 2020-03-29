# ===-- YamlTags.py - Objects that Represent YAML Tags -----------------------------------------------*- Python -*-=== #
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

import re
from pathlib import Path

from ruamel.yaml import SafeConstructor, yaml_object

rex_include_path = re.compile("<(?P<f>.*)>")


# ===--------------------------------------------------------------------------------------------------------------=== #

# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class FileMarker:
    def __init__(self, path: Path, start: str, end: str):
        self.marker = f"{path}:$:{start}:$:{end}"

    def __str__(self):
        return f"Mark for file: '{self.marker}'"


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class CustomSafeConstructor(SafeConstructor):

    def __init__(self, preserve_quotes=None, loader=None):
        super().__init__(preserve_quotes, loader)

    def construct_yaml_seq(self, node):
        data = self.yaml_base_list_type()
        yield data
        data.extend(self.construct_sequence(node))
        file_path = Path(node.start_mark.name).absolute()
        start_composite = f"{node.start_mark.line}:{node.start_mark.column}:{node.start_mark.index}"
        end_composite = f"{node.end_mark.line}:{node.end_mark.column}:{node.end_mark.index}"
        data.append(FileMarker(file_path, start_composite, end_composite))

    def construct_yaml_str(self, node):
        value = self.construct_scalar(node)
        try:
            value.encode('ASCII')
        except UnicodeEncodeError:
            raise SyntaxError(f"String values can only be standard ASCII strings,\n{node.start_mark}")
        return StringCType(value)


# ===--------------------------------------------------------------------------------------------------------------=== #

# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class YamlInclude:

    def __init__(self, value: str):
        self.path: str = value
        self.type: str = "???"

    def __str__(self):
        return self.path

    @classmethod
    def from_yaml(cls, constructor, node):
        def extract_path(string: str, mark) -> str:
            """
            Extract the path of a string (for YAML include files). The format is similar to the C include directive.

            :param string: a string to be matched against the path format
            :param mark: a mark to report the syntax error

            :return: a Path object with the computed path from the tag
            """
            path_matched = rex_include_path.match(string)
            if path_matched is not None:
                return Path(path_matched.group("f"))
            raise SyntaxError("Included YAML files must have to be valid paths files, enclosed between '<' and '>',"
                              "\n{mark}")

        return cls(extract_path(node.value, node.start_mark))


# noinspection PyPep8Naming,PyMissingOrEmptyDocstring,PyMissingTypeHints
def InitializeIncludeTags(yaml_parser):
    # noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
    @yaml_object(yaml_parser)
    class AbsoluteInclude(YamlInclude):
        """
        This class represent a path to include a file, relative to the root path directory.
        """
        yaml_tag = u'!$+'

        def __init__(self, value: str):
            super().__init__(value)
            self.type: str = "abs"

    # noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
    @yaml_object(yaml_parser)
    class RelativeInclude(YamlInclude):
        """
        This class represent a path to include a file, relative to the currently processed file.
        """
        yaml_tag = u'!$@'

        def __init__(self, value: str):
            super().__init__(value)
            self.type: str = "rel"

    # noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
    @yaml_object(yaml_parser)
    class WorkingDirectoryInclude(YamlInclude):
        """
        This class represent a path to include a file, relative to the current working directory.
        """
        yaml_tag = u'!$$'

        def __init__(self, value: str):
            super().__init__(value)
            self.type: str = "cwd"


# ===--------------------------------------------------------------------------------------------------------------=== #

# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class CType:
    """
    Represent all valid types of values for C.
    """

    def __eq__(self, other):
        if isinstance(other, type(self)):
            return self.reduced_value.__eq__(other.reduced_value)
        return NotImplemented

    def __hash__(self):
        return hash(self.reduced_value)

    def __init__(self, value: str):
        self.reduced_value: str = value

    def __str__(self):
        return self.reduced_value


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class UnsignedCType(CType):
    """
    This class represent an unsigned value. Unsigned values can be any value, but they must have to be unsigned
    integers. The children classes will restrict the type of the value.
    """

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
                raise SyntaxError(f"Can not parse a valid unsigned value for this {tag} tag,\n{mark}") from e

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
            string_format: str = "\\x01{}\\x02{}\\x03"
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
                raise SyntaxError(f"Can not parse a valid signed value for this {tag} tag,\n{mark}") from e

        return cls(format_signed(node.tag, node.value, node.start_mark))


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class StringCType(CType):
    """
    Represents a C string. This type of string is checked to be ASCII string.
    """


# noinspection PyPep8Naming,PyMissingOrEmptyDocstring,PyMissingTypeHints
def InitializeStronglyTypedTags(yaml_parser):
    # noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
    @yaml_object(yaml_parser)
    class Offset(SignedCType):
        """
        This class represent a pointer difference, which also means an offset or an array index. They are always signed
        integer constants.

        They are always of the machine's `ptrdiff_t` size.
        """
        yaml_tag = u'!offset'

    # noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
    @yaml_object(yaml_parser)
    class Pointer(UnsignedCType):
        """
        This class represent a pointer. Pointers are always unsigned integer constants.

        They are always of the machine's `uintptr_t` or `void *` size.
        """
        yaml_tag = u'!pointer'

    # noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
    @yaml_object(yaml_parser)
    class Range(CType):
        """
        This class represent a range. Ranges can contain any value inside them, but they must have to define a start, an
        end and a name.
        """
        yaml_tag = u'!range'

        @classmethod
        def from_yaml(cls, constructor, node):
            def reduce_range(tag: str, mapping_object: {}, mark) -> str:
                """
                Try to reduce the range tag to a string with a special format. Note that this is a generic range
                function, which can reduce any range (given the rules for range reduction).

                :param tag: the name of the tag
                :param mapping_object: the value of the tag
                :param mark: a mark to report the syntax error

                :return: a composite string with the value appended to the tag name
                """
                try:
                    description = mapping_object[StringCType("description")]
                    end = mapping_object[StringCType("end")]
                    start = mapping_object[StringCType("start")]
                    range_type = mapping_object[StringCType("type")]
                    # Raise an error if description is not a string
                    if type(description) is not StringCType:
                        raise SyntaxError(f"The 'description' property must have to be a string,\n{mark}")
                    # Raise an error if start and end are not the same type
                    if type(start) is not type(end):
                        raise SyntaxError(f"The 'start' and 'end' properties must have to be the same type,\n{mark}")
                    return f"\\x01{tag}\\x02{start}\\x1e{end}\\x1e" \
                           f"\\x02{range_type}\\x03\\x1e\\x02{description}\\x03\\x03"
                except KeyError as e:
                    raise SyntaxError(
                            f"The {tag} tag requires 4 properties: 'start', 'end', 'type' and 'description'.\n"
                            f"{mark}") from e

            mapping: {} = constructor.construct_mapping(node)
            return cls(reduce_range(node.tag, mapping, node.start_mark))

    # noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
    @yaml_object(yaml_parser)
    class SID(SignedCType):
        """
        This class represent a signed ID. Signed ID's are always signed integer constants.

        They are always of the machine's `signed` size.
        """
        yaml_tag = u'!s-id'

    # noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
    @yaml_object(yaml_parser)
    class Signed(SignedCType):
        """
        This class represent a signed value. Signed value are always signed integer constants.

        They are always of the machine's `signed` size.
        """
        yaml_tag = u'!signed'

    # noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
    @yaml_object(yaml_parser)
    class UID(UnsignedCType):
        """
        This class represent an unsigned ID. Unsigned ID's are always unsigned integer constants.

        They are always of the machine's `unsigned` size.
        """
        yaml_tag = u'!u-id'

    # noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
    @yaml_object(yaml_parser)
    class Unsigned(UnsignedCType):
        """
        This class represent an unsigned value. Unsigned values are always unsigned integer constants.

        They are always of the machine's `unsigned` size.
        """
        yaml_tag = u'!unsigned'


# ===--------------------------------------------------------------------------------------------------------------=== #

SafeConstructor.add_constructor(u'tag:yaml.org,2002:seq', CustomSafeConstructor.construct_yaml_seq)
SafeConstructor.add_constructor(u'tag:yaml.org,2002:str', CustomSafeConstructor.construct_yaml_str)

# ===--------------------------------------------------------------------------------------------------------------=== #
