# ===-- YamlMerging.py - Strategies for Merging YAML Objects -----------------------------------------*- Python -*-=== #
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

from typing import Any

# noinspection PyUnresolvedReferences
from YamlTags import FileMarker
from deepmerge import Merger
from deepmerge.strategy.dict import DictStrategies
from deepmerge.strategy.list import ListStrategies


# ===--------------------------------------------------------------------------------------------------------------=== #

# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class FileMarkedValue:
    def __init__(self, path: str, value: Any):
        self.marking = path
        self.contents = value

    def __str__(self):
        return f"{self.contents} from '{self.marking}'"

    def __eq__(self, other):
        if isinstance(other, type(self)):
            return self.marking.__eq__(other.marking) and self.contents.__eq__(other.contents)
        return NotImplemented


# ===--------------------------------------------------------------------------------------------------------------=== #

# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class YamlListStrategies(ListStrategies):
    """
    Contains the strategies provided for lists.
    """
    NAME = "list"

    @staticmethod
    def strategy_yaml_sequence(config, path: [], base: [], other: []):
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
        base_file_mark = base.pop()
        if type(base_file_mark) == FileMarker:
            base = [FileMarkedValue(base_file_mark.marker, x) for x in base]
        else:
            base.append(base_file_mark)
        other_file_mark = other.pop()
        if type(other_file_mark) == FileMarker:
            other = [FileMarkedValue(other_file_mark.marker, x) for x in other]
        else:
            other.append(other_file_mark)
        merged = []
        [merged.append(x) for x in base if x not in merged]
        [merged.append(x) for x in other if x not in merged]
        return merged


# noinspection PyMissingOrEmptyDocstring,PyMissingTypeHints,PyUnusedLocal
class YamlDictStrategies(DictStrategies):
    """
    Contains the strategies provided for dictionaries.
    """
    NAME = "dict"

    @staticmethod
    def strategy_yaml_mapping(config, path: {}, base: {}, other: {}):
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
class YamlMerger(Merger):
    """
    :param type_strategies, List[Tuple]: a list of (Type, Strategy) pairs that should be used against incoming types.
    """
    PROVIDED_TYPE_STRATEGIES = {
        list: YamlListStrategies,
        dict: YamlDictStrategies
    }


# noinspection PyPep8Naming,PyMissingOrEmptyDocstring,PyMissingTypeHints
def GetYamlMerger():
    return YamlMerger([(list, "yaml_sequence"), (dict, "yaml_mapping")], ["override"], [])

# ===--------------------------------------------------------------------------------------------------------------=== #
