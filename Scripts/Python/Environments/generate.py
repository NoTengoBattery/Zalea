# ===-- generate.py - Generate a Python 3 virtual environment  ---------------------------------------*- Python -*-=== #
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
# SPDX-License-Identifier: Apache-2.0
#
# ===--------------------------------------------------------------------------------------------------------------=== #
# /
# / \file
# / This is an ad-hoc script that is meant to be run by CMake when it initializes the cache. This file will generate a
# / clean Virtual Environment with all the tools needed to run the CMake Language Extensions, without relying on the
# / host Python 3.
# /
# ===--------------------------------------------------------------------------------------------------------------=== #
import argparse
import os
import subprocess
import sys
import threading
import urllib.parse
import urllib.request
import venv


class ExtendedEnvBuilder(venv.EnvBuilder):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def post_setup(self, context):
        os.environ['VIRTUAL_ENV'] = context.env_dir
        self.install_pip(context)

    @staticmethod
    def reader(stream, context):
        while True:
            s = stream.readline()
            if not s:
                break
            sys.stderr.write(s.decode('utf-8'))
            sys.stderr.flush()
        stream.close()

    def install_script(self, context, name, url):
        _, _, path, _, _, _ = urllib.parse.urlparse(url)
        fn = os.path.split(path)[-1]
        bin_path = context.bin_path
        dist_path = os.path.join(bin_path, fn)
        urllib.request.urlretrieve(url, dist_path)
        sys.stderr.write('Installing %s...%s' % (name, '\n'))
        sys.stderr.flush()
        args = [context.env_exe, fn]
        p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=bin_path)
        t1 = threading.Thread(target=self.reader, args=(p.stdout, 'stdout'))
        t2 = threading.Thread(target=self.reader, args=(p.stderr, 'stderr'))
        t1.start()
        t2.start()
        p.wait()
        t1.join()
        t2.join()
        os.unlink(dist_path)

    def install_pip(self, context):
        url = 'https://bootstrap.pypa.io/get-pip.py'
        self.install_script(context, 'pip', url)
        # Install the latest packages. If the latest breaks the scripts, we should update the scripts instead of
        # installing a specific version of the packages.
        subprocess.check_call([context.env_exe, "-m", "pip", "install", "pytest"])
        subprocess.check_call([context.env_exe, "-m", "pip", "install", "tinydb"])


class FileLock:
    def __init__(self, filename):
        self.filename = filename
        self.fd = None
        self.pid = os.getpid()

    def acquire(self):
        try:
            self.fd = os.open(self.filename, os.O_CREAT | os.O_EXCL | os.O_RDWR)
            os.write(self.fd, str.encode("%d" % self.pid))
            return 1
        except OSError:
            self.fd = None
            return 0

    def release(self):
        if not self.fd:
            return 0
        try:
            os.close(self.fd)
            os.remove(self.filename)
            return 1
        except OSError:
            return 0

    def __del__(self):
        self.release()


def main(args=None):
    compatible = True
    if sys.version_info < (3, 7):
        compatible = False
    elif not hasattr(sys, 'base_prefix'):
        compatible = False
    if not compatible:
        raise ValueError('This script is only for use with Python 3.7 or later.')
    else:
        parser = argparse.ArgumentParser(prog=__name__,
                                         description='Creates virtual Python environments in one or more target '
                                                     'directories.')
        parser.add_argument('dirs', metavar='ENV_DIR', nargs='+', help='A directory to create the environment in.')
    if os.name == 'nt':
        symlinks = False
    else:
        symlinks = True
    options = parser.parse_args(args)
    builder = ExtendedEnvBuilder(symlinks=symlinks)
    for d in options.dirs:
        builder.create(d)


if __name__ == '__main__':
    try:
        if not os.path.isfile('done.txt'):
            lock = FileLock("lock.file")
            if lock.acquire():
                main()
                open("done.txt", 'a').close()
                exit(0)
            else:
                exit(100)
        else:
            exit(0)
    except Exception as e:
        print('Error: %s' % e, file=sys.stderr)
        sys.exit(200)
