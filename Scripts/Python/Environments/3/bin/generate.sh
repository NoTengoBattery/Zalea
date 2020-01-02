#!/usr/bin/env sh
VROOT=$1

cd "$VROOT" || exit
python3 -m venv .
pip install --upgrade pip
pip install --upgrade setuptools || pip install setuptools
./bin/pip install tinydb
