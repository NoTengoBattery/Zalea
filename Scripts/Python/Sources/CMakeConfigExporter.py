# ===-- CMakeConfigExporter.py - Configuration Exporter for CMake ------------------------------------*- Python -*-=== #
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
# / This is a CMake Language Extension. This extension allows the build system to generate an "initial cache" file for
# / CMake, which will populate all the variables in order to reproduce a build. This way, you can generate "default
# / configurations" (properly known as "configuration seeds") for your targets, or distribute the file in order to
# / allow users to generate a binary copy of the project.
# /
# ===--------------------------------------------------------------------------------------------------------------=== #

import argparse
import io
import logging
import sys

from tinydb import JSONStorage
from tinydb import TinyDB
from tinydb import where
from tinydb.middlewares import CachingMiddleware

default_json_filename = 'config.i.json'
program_parser = None
parsed_arguments = None
buffer_size = 64 * 1024  # 64kib
database_file = None
tinydb_database = None
program_logger = logging.getLogger(__name__)


def parse_args(args):
    """
    Parse the program's arguments that control the program's behaviour.
    :param args: arguments to parse
    :return: the parsed arguments
    """
    parser = argparse.ArgumentParser(
            description='Configuration cache fixer. This program fixes and updates a CMake file that contains exported '
                        'cache entries.',
            epilog='This script was designed to extend the CMake language. Even if this file can be call and used '
                   'directly, you should avoid that. Initializing the JSON database will always purge it. All changes '
                   'made to the JSON database will be lost upon INT command. The VALUE and the DOCSTRING will be '
                   'updated always. The TYPE of the variable will be set on creation and never updated.')
    parser.add_argument('action',
                        action='store', default='SAE', type=str, metavar='action',
                        help='Selects the action to take in the file. '
                             'INT=INIT_CONFIG; '
                             'SAE=SET_AND_EXPORT; '
                             'END=DUMP_CONFIG. '
                             '(default: %(default)s; type: %(type)s).',
                        choices=['SAE', 'INT', 'END'])
    parser.add_argument('-f', '--file',
                        action='store', type=argparse.FileType(mode='w', bufsize=buffer_size), metavar='file',
                        help='The destination of the cache file. This file is created and dumped at the end of the '
                             'configuration process, when the script is called with the action "END".')
    parser.add_argument('-d', '--dbfile',
                        action='store', type=argparse.FileType(mode='a+', bufsize=buffer_size), metavar='db',
                        default=default_json_filename,
                        help='The destination of the database file. This is the IR for the script. All changes are '
                             'committed to this DB before the DB is dumped and formatted in the "END" action. '
                             '(default: %(default)s).')
    parser.add_argument('-v', '--variable',
                        action='store', type=str, metavar='variable',
                        help='A CMake variable name to add in the database. This value cannot be empty.')
    parser.add_argument('-a', '--value',
                        action='store', type=str, metavar='value', default="",
                        help='The value of the corresponding CMake variable. Always treated as a string. Can be '
                             'empty. If the variable is type BOOL, you can only use ON or OFF, they will be treated '
                             'as bool values, any invalid value is treated as OFF.')
    parser.add_argument('-t', '--type',
                        action='store', type=str, metavar='type', default="STRING",
                        help='The type of the CMake variable. The type is always uppercase and can be any of the '
                             'valid CMake cache variable types. This value cannot be empty.',
                        choices=['BOOL', 'FILEPATH', 'PATH', 'STRING', 'INTERNAL'])
    parser.add_argument('-u', '--default',
                        action='store', type=str, metavar='default', default="",
                        help='A default value for the CMake variable. If this value is identical to the given value, '
                             'then the variable is not added to the CMake file. It is added to the CMake Header file.')
    parser.add_argument('-o', '--docstring',
                        action='store', type=str, metavar='docstring', default="",
                        help='An optional docstring to describe the variable in the CMake cache. Can be empty.')
    parser.add_argument('-e', '--template',
                        action='store', type=argparse.FileType(bufsize=buffer_size), metavar='templ',
                        help='A "template" file that will be opened for read, read and then appended to the beginning '
                             'of the output CMake file. This file must exist. This value cannot be empty.')
    parser.add_argument('-x', '--header',
                        action='store_true',
                        help='When using this option, this script will generate a CMake Header file, which CMake then'
                             'will configure to create a valid include file for C/C++ with all options in the '
                             'database.')
    parser.add_argument('-r', '--force',
                        action='store_true',
                        help='Using this flag with a new entry causes it to use the SET_AND_EXPORT_FORCE command. '
                             'Updating an entry does not change it\'s force status.')
    parser.add_argument('-b', '--verbose',
                        action='store_true',
                        help='Using this flag will print debug messages from the logger to the console by default.')
    global program_parser
    global parsed_arguments
    program_parser = parser
    parsed_arguments = parser.parse_args(args)
    return parsed_arguments


def open_db(args):
    """
    Open the TinyDB database to be used inside the Python script.
    :param args: parsed arguments which contains the database file name.
    """
    global database_file
    global tinydb_database
    database_file = str(args.dbfile.name)
    program_logger.debug("Opening database file in %s" % database_file)
    if not database_file:
        program_logger.critical("Aborting because the database file name was not provided")
        raise SystemExit(10, 'Cannot run this script without the reference to the DB file.')
    tinydb_database = TinyDB(database_file, storage=CachingMiddleware(JSONStorage))
    program_logger.info("TinyDB file opened!")


def init_db(args):
    """
    Initialize the database by purging all it's contents and tables.
    :param args: parsed arguments which contains the database file name.
    """
    open_db(args)
    with tinydb_database as db:
        db.purge()
        db.purge_tables()
        program_logger.info("Initialized database!")
        program_logger.info("The database and all it's tables are empty now")


def set_and_export(args):
    """
    Perform the "set and export" step of the process.
    :param args: parsed arguments which defines the script behaviour.
    """
    # Verify the flags
    if not args.variable or not args.type:
        program_parser.error(
                'For updating or inserting an entry you must provide, at least, the type and the variable name')
    open_db(args)
    with tinydb_database as db:
        # This code is way too bad. I know it but it is intentional. No need to optimize or generalize as:
        # 1. No new options are intended to be added
        # 2. The format and values are already fixed and should not change
        db_table_f = db.table('SET_FORCE')
        db_table_nf = db.table('SET')
        entry_f = db_table_f.search(where('variable') == args.variable)
        entry_nf = db_table_nf.search(where('variable') == args.variable)
        # If one entry exist in both tables, remove both
        if entry_f and entry_nf:
            db_table_nf.remove(where('variable') == args.variable)
            db_table_f.remove(where('variable') == args.variable)
        if entry_f:
            db_table = db_table_f
            entry = entry_f
            program_logger.debug("Entry for variable '%s' found on table '%s'" % (args.variable, db_table.name))
        elif entry_nf:
            db_table = db_table_nf
            entry = entry_nf
            program_logger.debug("Entry for variable '%s' found on table '%s'" % (args.variable, db_table.name))
        elif args.force:
            db_table = db_table_f
            entry = False
            program_logger.debug("No entry found. Using the table '%s' for insert" % db_table.name)
        elif not args.force:
            db_table = db_table_nf
            entry = False
            program_logger.debug("No entry found. Using the table '%s' for insert" % db_table.name)
        else:
            raise SystemExit(12, 'Cannot choose a valid table to insert or update.')
        # If the variable exist, only update the value. Delete if it's value is the default.
        if entry:
            db_table.update({
                'value': args.value,
                'default': args.default,
                'docstring': args.docstring
            }, where('variable') == args.variable)
            program_logger.info("Update entry: {value: '%s', docstring: '%s'}" % (args.value, args.docstring))
        else:
            db_table.insert({
                'variable': args.variable,
                'value': args.value,
                'default': args.default,
                'type': args.type,
                'docstring': args.docstring})
            program_logger.info("Add entry: {variable: '%s', value: '%s', type: '%s', docstring: '%s'}"
                                % (args.variable, args.value, args.type, args.docstring))


def dump_table(table: str, file, header, default):
    """
    Perform the table dump. This function will dump the tables inside the database according to it's respective format.
    :param table: the table to dump.
    :param file: the file where the result will be dumped.
    :param header: a header to include inside the dumped resulting file.
    :param default: default.
    """
    if header:
        program_logger.info("Ready to dump database to CMake Header file")
        with tinydb_database as db:
            db_table = db.table(table)
            data = db_table.all()
            dicts = {data[i]['variable']: data[i] for i in range(0, len(data))}
            for key in sorted(dicts):
                doc = dicts[key]
                if table == 'SET':
                    msg = 'SET_AND_EXPORT'
                elif table == 'SET_FORCE':
                    msg = 'SET_AND_EXPORT_FORCE'
                else:
                    raise SystemExit(13, 'Invalid table was provided!')
                file.write('/* {}: {} {} */\n'
                           .format(key, msg, doc['type']))
                if doc['type'] == 'BOOL':
                    file.write('#cmakedefine\t{}  // NOLINT \n'
                               .format(key))
                else:
                    file.write('#cmakedefine\t{}\t@{}@  // NOLINT \n'
                               .format(key, key))
                program_logger.debug("Dumped CMake entry for variable '%s'" % key)
    else:
        program_logger.info("Ready to dump database to CMake file")
        with tinydb_database as db:
            db_table = db.table(table)
            data = db_table.all()
            dicts = {data[i]['variable']: data[i] for i in range(0, len(data))}
            for key in sorted(dicts):
                doc = dicts[key]
                if doc['value'] != doc['default']:
                    if table == 'SET':
                        file.write('# {}: {} {}\n'
                                   .format(key, 'SET_AND_EXPORT', doc['type']))
                        file.write('SET({} \"{}\"\n\tCACHE {}\n\t\"{}\")\n'.expandtabs(2)
                                   .format(key, doc['value'], doc['type'], doc['docstring']))
                    elif table == 'SET_FORCE':
                        file.write('# {}: {} {}\n'
                                   .format(key, 'SET_AND_EXPORT_FORCE', doc['type']))
                        file.write('SET({} \"{}\"\n\tCACHE {}\n\t\"{}\" FORCE)\n'.expandtabs(2)
                                   .format(key, doc['value'], doc['type'], doc['docstring']))
                    else:
                        raise SystemExit(13, 'Invalid table was provided!')
                    program_logger.debug("Dumped CMake entry for variable '%s'" % key)
                else:
                    program_logger.debug("Ignoring variable '%s' with default value" % key)


def dump_db(args):
    """
    Dump the whole database, including all of it's tables.
    :param args: parsed arguments which defines the script behaviour.
    """
    # Verify the flags
    if not args.template or not args.file:
        program_parser.error(
                'Dumping the cache to a CMake configuration file requires a template and the destination file')
    open_db(args)
    # Dump the contents of the template file to the actual file
    with io.StringIO("") as output:
        with args.template as template:
            output.write(template.read())
            program_logger.info("Copied template file to output file")
        output.write('\n')
        dump_table('SET', output, args.header, args.default)
        program_logger.info("Dumped SET table to output file")
        dump_table('SET_FORCE', output, args.header, args.default)
        program_logger.info("Dumped SET_FORCE table to output file")
        with args.file as real_output:
            output.seek(0)
            real_output.write(output.read())
            program_logger.info("File '%s' written to disk" % real_output.name)


def main(args):
    """
    Main program (entry point).
    :param args: arguments from command line
    """
    global program_logger
    parsed = parse_args(args)
    if program_parser is None:
        raise SystemExit(11, 'Program argument parser not set or global argument set is None.')
    if parsed_arguments is None:
        raise SystemExit(14, 'Parsed program arguments is None.')
    if parsed.verbose:
        program_logger.setLevel(logging.DEBUG)
        program_logger.info("Logger level changed to DEBUG")
    if parsed.action == 'INT':
        init_db(parsed)
        program_logger.info("Operation INT")
    elif parsed.action == 'SAE':
        set_and_export(parsed)
        program_logger.info("Operation SAE")
    elif parsed.action == 'END':
        dump_db(parsed)
        program_logger.info("Operation END")


if __name__ == '__main__':
    main(sys.argv[1:])
