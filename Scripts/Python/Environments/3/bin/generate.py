#!/usr/bin/env python3
import argparse
import os
import sys
import venv
from subprocess import PIPE, Popen
from threading import Thread
from urllib.parse import urlparse
from urllib.request import urlretrieve


class ExtendedEnvBuilder(venv.EnvBuilder):
    def __init__(self, *args, **kwargs):
        self.nodist = False
        self.nopip = False
        super().__init__(*args, **kwargs)

    def post_setup(self, context):
        os.environ['VIRTUAL_ENV'] = context.env_dir
        self.install_pip(context)

    def reader(self, stream, context):
        while True:
            s = stream.readline()
            if not s:
                break
            sys.stderr.write(s.decode('utf-8'))
            sys.stderr.flush()
        stream.close()

    def install_script(self, context, name, url):
        _, _, path, _, _, _ = urlparse(url)
        fn = os.path.split(path)[-1]
        bin_path = context.bin_path
        dist_path = os.path.join(bin_path, fn)
        urlretrieve(url, dist_path)
        term = '\n'
        sys.stderr.write('Installing %s ...%s' % (name, term))
        sys.stderr.flush()
        args = [context.env_exe, fn]
        p = Popen(args, stdout=PIPE, stderr=PIPE, cwd=bin_path)
        t1 = Thread(target=self.reader, args=(p.stdout, 'stdout'))
        t1.start()
        t2 = Thread(target=self.reader, args=(p.stderr, 'stderr'))
        t2.start()
        p.wait()
        t1.join()
        t2.join()
        sys.stderr.write('done.\n')
        os.unlink(dist_path)

    def install_pip(self, context):
        url = 'https://bootstrap.pypa.io/get-pip.py'
        self.install_script(context, 'pip', url)

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
        builder = ExtendedEnvBuilder(symlinks=options.symlinks)
        for d in options.dirs:
            builder.create(d)


if __name__ == '__main__':
    rc = 1
    try:
        venv.main()
        rc = 0
    except Exception as e:
        print('Error: %s' % e, file=sys.stderr)
    sys.exit(rc)
