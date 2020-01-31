# ===-- TestCMakeConfigExporter.py - Test the Configuration Exporter for CMake -----------------------*- Python -*-=== #
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
# / This file will test the correctness of the CMakeConfigExporter.py script.
# /
# ===--------------------------------------------------------------------------------------------------------------=== #
import os

import pytest

from Scripts.Python.Sources.CMakeConfigExporter import *


@pytest.fixture(scope="session")
def db_file(tmpdir_factory):
    fn = tmpdir_factory.mktemp("db").join("tstdb.i.json")
    return str(fn)


@pytest.fixture(scope="session")
def cmake_file(tmpdir_factory):
    fn = tmpdir_factory.mktemp("db").join("tst.cmake")
    return str(fn)


@pytest.fixture(scope="session")
def cmake_header(tmpdir_factory):
    fn = tmpdir_factory.mktemp("db").join("tst.h")
    return str(fn)


@pytest.fixture(scope="session")
def template_file(tmpdir_factory):
    fn = tmpdir_factory.mktemp("db").join("template.cmake")
    ff = open(fn, mode="w")
    ff.write('####')
    return str(fn)


@pytest.fixture(scope="session")
def template_header(tmpdir_factory):
    fn = tmpdir_factory.mktemp("db").join("template.header")
    ff = open(fn, mode="w")
    ff.write('//////')
    return str(fn)


def test_h_arg():
    with pytest.raises(SystemExit) as exception:
        args = ['-h']
        main(args)
    assert exception.type == SystemExit
    assert exception.value.code == 0


def test_help_arg():
    with pytest.raises(SystemExit) as exception:
        args = ['--help']
        main(args)
    assert exception.type == SystemExit
    assert exception.value.code == 0


def test_invalid_action():
    with pytest.raises(SystemExit) as exception:
        args = ['INVALID']
        main(args)
    assert exception.type == SystemExit
    assert exception.value.code == 2


def test_init_db(db_file):
    args = ['INT', "--dbfile", db_file]
    main(args)
    assert os.path.exists(db_file) == 1


def test_init_db_deffile():
    args = ['INT']
    main(args)
    assert os.path.exists(default_db_filename) == 1


def test_insert_no_varname():
    with pytest.raises(SystemExit) as exception:
        args = ['SAE',
                '--value', 'TEST',
                '--type', 'STRING',
                '--docstring', 'Fancy description']
        main(args)
    assert exception.type == SystemExit
    assert exception.value.code == 2


def test_insert_no_type():
    args = ['SAE',
            '--value', 'TEST',
            '--variable', 'CMAKE_TEST',
            '--docstring', 'Fancy description']
    main(args)


def test_insert_invalid_type():
    with pytest.raises(SystemExit) as exception:
        args = ['SAE',
                '--variable', 'CMAKE_TEST',
                '--value', 'TEST',
                '--type', 'INVALID_TYPE',
                '--docstring', 'Fancy description']
        main(args)
    assert exception.type == SystemExit
    assert exception.value.code == 2


def test_value_insertion():
    """
    This test hard-codes the DB structure. This is absolutely intended, a bad practice but intentional. We can rely
    in a format for the DB if we need to use other tools.
    """
    args = ['INT']
    main(args)

    args = ['SAE',
            '--variable', 'IVAR',
            '--value', 'IVAL',
            '--type', 'STRING',
            '--docstring', 'DESC']
    main(args)
    # Open the raw DB
    # Access the SET table
    # Query the just inserted value
    raw_db = TinyDB(default_db_filename)
    table_db = raw_db.table('SET')
    entry = table_db.search(where('variable') == 'IVAR')
    # Test: Not none, values are correct
    assert entry is not None
    first = entry[0]
    assert first['variable'] == 'IVAR'
    assert first['value'] == 'IVAL'
    assert first['type'] == 'STRING'
    assert first['docstring'] == 'DESC'


def test_value_insertion_with_default():
    """
    This test hard-codes the DB structure. This is absolutely intended, a bad practice but intentional. We can rely
    in a format for the DB if we need to use other tools.
    """
    args = ['INT']
    main(args)

    args = ['SAE',
            '--variable', 'IVAR',
            '--value', 'IVAL',
            '--type', 'STRING',
            '--default', 'IVAL',
            '--docstring', 'DESC']
    main(args)
    # Open the raw DB
    # Access the SET table
    # Query the just inserted value
    raw_db = TinyDB(default_db_filename)
    table_db = raw_db.table('SET')
    entry = table_db.search(where('variable') == 'IVAR')
    # Test: Not none, values are correct
    assert entry is not None
    first = entry[0]
    assert first['variable'] == 'IVAR'
    assert first['value'] == 'IVAL'
    assert first['type'] == 'STRING'
    assert first['default'] == 'IVAL'
    assert first['docstring'] == 'DESC'


def test_value_insertion_with_default_update():
    """
    This test hard-codes the DB structure. This is absolutely intended, a bad practice but intentional. We can rely
    in a format for the DB if we need to use other tools.
    """
    args = ['INT']
    main(args)

    args = ['SAE',
            '--variable', 'IVAR',
            '--value', 'IVAL',
            '--type', 'STRING',
            '--docstring', 'DESC']
    main(args)
    # Open the raw DB
    # Access the SET table
    # Query the just inserted value
    raw_db = TinyDB(default_db_filename)
    table_db = raw_db.table('SET')
    entry = table_db.search(where('variable') == 'IVAR')
    # Test: Not none, values are correct
    assert entry is not None
    first = entry[0]
    assert first['variable'] == 'IVAR'
    assert first['value'] == 'IVAL'
    assert first['type'] == 'STRING'
    assert first['docstring'] == 'DESC'
    args = ['SAE',
            '--variable', 'IVAR',
            '--value', 'IVAL',
            '--type', 'STRING',
            '--default', 'IVAL',
            '--docstring', 'DESC']
    main(args)
    raw_db = TinyDB(default_db_filename)
    table_db = raw_db.table('SET')
    entry = table_db.search(where('variable') == 'IVAR')
    # Test: Not none, values are correct
    assert entry is not None
    first = entry[0]
    assert first['variable'] == 'IVAR'
    assert first['value'] == 'IVAL'
    assert first['type'] == 'STRING'
    assert first['default'] == 'IVAL'
    assert first['docstring'] == 'DESC'


def test_value_update(caplog, template_file, cmake_file):
    """
    This test hard-codes the DB structure. This is absolutely intended, a bad practice but intentional. We can rely
    in a format for the DB if we need to use other tools.
    """
    with caplog.at_level(logging.DEBUG):
        args = ['INT']
        main(args)

        args = ['SAE',
                '--variable', 'IVAR',
                '--value', 'IVAL',
                '--type', 'STRING',
                '--verbose',
                '--docstring', 'DESC']
        main(args)

        args = ['SAE',
                '--variable', 'IVAR',
                '--value', 'NVAL',
                '--type', 'BOOL',
                '--verbose',
                '--docstring', 'NDESC']
        main(args)

        args = ['END',
                '--template', str(template_file),
                '--file', str(cmake_file),
                '--verbose']
        main(args)

        # Open the raw DB
        # Access the SET table
        # Query the just inserted value
        raw_db = TinyDB(default_db_filename)
        table_db = raw_db.table('SET')
        entry = table_db.search(where('variable') == 'IVAR')
        # Test: Not none, values are correct
        assert entry is not None
        first = entry[0]
        assert first['variable'] == 'IVAR'
        assert first['value'] == 'NVAL'
        assert first['type'] == 'STRING'
        assert first['docstring'] == 'NDESC'


def test_value_insert_force():
    """
    This test hard-codes the DB structure. This is absolutely intended, a bad practice but intentional. We can rely
    in a format for the DB if we need to use other tools.
    """
    args = ['INT']
    main(args)

    args = ['SAE',
            '--variable', 'IVAR',
            '--value', 'IVAL',
            '--type', 'STRING',
            '--docstring', 'DESC',
            '--force']
    main(args)
    # Open the raw DB
    # Access the SET_FORCE table
    # Query the just inserted value
    raw_db = TinyDB(default_db_filename)
    table_db = raw_db.table('SET_FORCE')
    entry = table_db.search(where('variable') == 'IVAR')
    # Test: Not none, values are correct
    assert entry is not None
    first = entry[0]
    assert first['variable'] == 'IVAR'
    assert first['value'] == 'IVAL'
    assert first['type'] == 'STRING'
    assert first['docstring'] == 'DESC'


def test_value_update_force():
    """
    This test hard-codes the DB structure. This is absolutely intended, a bad practice but intentional. We can rely
    in a format for the DB if we need to use other tools.
    """
    args = ['INT']
    main(args)

    args = ['SAE',
            '--variable', 'IVAR',
            '--value', 'IVAL',
            '--type', 'STRING',
            '--docstring', 'DESC',
            '--force']
    main(args)

    args = ['SAE',
            '--variable', 'IVAR',
            '--value', 'NVAL',
            '--type', 'BOOL',
            '--docstring', 'NDESC']
    main(args)

    # Open the raw DB
    # Access the SET_FORCE table
    # Query the just inserted value
    raw_db = TinyDB(default_db_filename)
    table_db = raw_db.table('SET_FORCE')
    entry = table_db.search(where('variable') == 'IVAR')
    # Test: Not none, values are correct
    assert entry is not None
    first = entry[0]
    assert first['variable'] == 'IVAR'
    assert first['value'] == 'NVAL'
    assert first['type'] == 'STRING'
    assert first['docstring'] == 'NDESC'


def test_dump_no_template(cmake_file):
    with pytest.raises(SystemExit) as exception:
        args = ['END',
                '--file', str(cmake_file)]
        main(args)
    assert exception.type == SystemExit
    assert exception.value.code == 2


def test_dump_no_destination(template_file):
    with pytest.raises(SystemExit) as exception:
        args = ['END',
                '--template', str(template_file)]
        main(args)
    assert exception.type == SystemExit
    assert exception.value.code == 2


def test_dumped_format(db_file, template_file, cmake_file):
    """
    This test hard-codes the format of the output file. This is important because we can rely on the file format to
    feed another tools and that this script will always create a valid CMake file.
    """
    args = ['INT',
            '--dbfile', db_file]
    main(args)

    args = ['SAE',
            '--variable', 'V',
            '--value', 'A',
            '--type', 'BOOL',
            '--docstring', 'D',
            '--dbfile', db_file]
    main(args)

    args = ['SAE',
            '--variable', 'E',
            '--value', 'U',
            '--type', 'BOOL',
            '--docstring', 'G',
            '--dbfile', db_file]
    main(args)

    args = ['SAE',
            '--variable', 'EL',
            '--value', 'EU',
            '--type', 'BOOL',
            '--docstring', 'GN',
            '--force',
            '--dbfile', db_file]
    main(args)

    args = ['SAE',
            '--variable', 'VA',
            '--value', 'AL',
            '--type', 'BOOL',
            '--docstring', 'DO',
            '--force',
            '--dbfile', db_file]
    main(args)

    args = ['SAE',
            '--variable', 'TR',
            '--value', 'UL',
            '--default', 'UL',
            '--type', 'BOOL',
            '--docstring', 'MU',
            '--dbfile', db_file]
    main(args)

    args = ['END',
            '--template', str(template_file),
            '--file', str(cmake_file),
            '--dbfile', db_file]
    main(args)

    with open(cmake_file) as f:
        assert str(f.read()) == '####\n' \
                                '# E: SET_AND_EXPORT BOOL\n' \
                                'SET(E "U"\n' \
                                '  CACHE BOOL\n' \
                                '  "G")\n' \
                                '# V: SET_AND_EXPORT BOOL\n' \
                                'SET(V "A"\n' \
                                '  CACHE BOOL\n' \
                                '  "D")\n' \
                                '# EL: SET_AND_EXPORT_FORCE BOOL\n' \
                                'SET(EL "EU"\n' \
                                '  CACHE BOOL\n' \
                                '  "GN" FORCE)\n' \
                                '# VA: SET_AND_EXPORT_FORCE BOOL\n' \
                                'SET(VA "AL"\n' \
                                '  CACHE BOOL\n' \
                                '  "DO" FORCE)\n'


def test_dumped_format_header(db_file, template_header, cmake_header):
    """
    This test hard-codes the format of the output file. This is important because we can rely on the file format to
    feed another tools and that this script will always create a valid CMake file.
    """
    args = ['INT',
            '--dbfile', db_file]
    main(args)

    args = ['SAE',
            '--variable', 'V',
            '--value', 'A',
            '--type', 'BOOL',
            '--docstring', 'D',
            '--dbfile', db_file]
    main(args)

    args = ['SAE',
            '--variable', 'E',
            '--value', 'U',
            '--type', 'BOOL',
            '--docstring', 'G',
            '--dbfile', db_file]
    main(args)

    args = ['SAE',
            '--variable', 'EL',
            '--value', 'EU',
            '--type', 'BOOL',
            '--docstring', 'GN',
            '--force',
            '--dbfile', db_file]
    main(args)

    args = ['SAE',
            '--variable', 'VA',
            '--value', 'AL',
            '--type', 'BOOL',
            '--docstring', 'DO',
            '--force',
            '--dbfile', db_file]
    main(args)

    args = ['END',
            '--header',
            '--template', str(template_header),
            '--file', str(cmake_header),
            '--dbfile', db_file]
    main(args)

    with open(cmake_header) as f:
        assert str(f.read()) == '//////\n' \
                                '/* E: SET_AND_EXPORT BOOL */\n' \
                                '#cmakedefine\tE\n' \
                                '/* V: SET_AND_EXPORT BOOL */\n' \
                                '#cmakedefine\tV\n' \
                                '/* EL: SET_AND_EXPORT_FORCE BOOL */\n' \
                                '#cmakedefine\tEL\n' \
                                '/* VA: SET_AND_EXPORT_FORCE BOOL */\n' \
                                '#cmakedefine\tVA\n'


def test_large_loop(db_file, template_file, cmake_file):
    args = ['INT',
            '--dbfile', db_file]
    main(args)
    for i in range(1 * 512):
        args = ['SAE',
                '--variable', str(i),
                '--value', 'A' + str(i),
                '--type', 'STRING',
                '--docstring', 'D' + str(i),
                '--dbfile', db_file]
        main(args)
        args = ['SAE',
                '--variable', 'F' + str(i),
                '--value', 'FA' + str(i),
                '--type', 'STRING',
                '--docstring', 'FD' + str(i),
                '--force',
                '--dbfile', db_file]
        main(args)
    args = ['END',
            '--template', str(template_file),
            '--file', str(cmake_file),
            '--dbfile', db_file]
    main(args)
