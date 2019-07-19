#!/usr/bin/python
# vim: set fileencoding=utf-8
import clang.cindex
import asciitree # must be version 0.2
import sys

def node_children(node):
    return list(c for c in node.get_children() if c is not None and c.location is not None and c.location.file is not None and c.location.file.name == sys.argv[1])

def print_node(node):
    text = node.spelling or node.displayname
    kind = str(node.kind)[str(node.kind).index('.')+1:]
    if clang.cindex.CursorKind.FIELD_DECL == node.kind:
        size = node.type.get_size()
        return '{} {} {}'.format(kind, text, size)
    return '{} {}'.format(kind, text)

if len(sys.argv) < 2:
    print("Usage: dump_ast.py [header file name] [additional compile args]")
    sys.exit()

clang.cindex.Config.set_library_file('/usr/local/Cellar/llvm/6.0.0/lib/libclang.dylib')
index = clang.cindex.Index.create()
translation_unit = index.parse(sys.argv[1], ['-x', 'c++', '-std=c++11'] + sys.argv[2:])

print(asciitree.draw_tree(translation_unit.cursor, node_children, print_node))
