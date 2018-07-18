#!/usr/bin/env python

from __future__ import print_function
import argparse
import sys
import json
import textwrap
from collections import OrderedDict
try:
    import yaml
except ImportError:
    yaml = None

VERSION = "0.1"


if yaml:
    def yaml_load(stream, Loader=yaml.Loader, object_pairs_hook=OrderedDict):
        class OrderedLoader(Loader):
            pass

        def construct_mapping(loader, node):
            loader.flatten_mapping(node)
            return object_pairs_hook(loader.construct_pairs(node))

        OrderedLoader.add_constructor(
            yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG,
            construct_mapping)
        return yaml.load(stream, OrderedLoader)


def walk(node, cb):

    if isinstance(node, dict):
        for key, item in node.items():
            cb(key, item)
            walk(item, cb)
    elif type(node) is list:
        for item in iter(node):
            cb(None, item)
            walk(item, cb)


def print_body(key, item):

    if type(item) is OrderedDict and \
       'element' in item.keys() and \
       item['element'] == 'asset':
        print(item['content'])


def main():
    parser = argparse.ArgumentParser(
        description=textwrap.dedent('''\
    Simple filter for refract, prints out the json and JSONSchema content
    of the datastrucutres. Input is either stdin or given files.'''),
        epilog=textwrap.dedent('''\
    Example:
        refract-filter.py -vj test/fixtures/schema/*.json
        drafter blueprint.apib | refract-filter.py'''),
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-j', '--json',
                        help='input is json format and not yaml',
                        action='store_true', default=False)
    parser.add_argument('-V', '--version', help='print version info',
                        action='store_true', default=False)
    parser.add_argument('-v', '--verbose', help='verbose (print file names)',
                        action='store_true', default=False)
    parser.add_argument('file', type=argparse.FileType('r'), nargs='*')
    args = parser.parse_args()

    if yaml is None:
        args.json = True
        if args.verbose:
            print("Pyaml not found, only json format supported, -j in effect",
                  file=sys.stderr)

    if args.version:
        print(VERSION + " refract-filter.py")
        sys.exit(0)

    if not args.file:
        args.file.append(sys.stdin)

    for f in args.file:
        if args.verbose:
            print(f.name, file=sys.stderr)
        if args.json:
            data = json.load(f, object_pairs_hook=OrderedDict)
        else:
            data = yaml_load(f)

        walk(data, print_body)


if __name__ == '__main__':
    main()
