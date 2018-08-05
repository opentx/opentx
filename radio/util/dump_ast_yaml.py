#!/usr/bin/python
# vim: set fileencoding=utf-8

from clang.cindex import *
import asciitree # must be version 0.2
import sys

TOP_NODE_TYPES = [
    CursorKind.ENUM_DECL,
    CursorKind.STRUCT_DECL ]

CHILD_NODE_TYPES = [
    CursorKind.FIELD_DECL,
    CursorKind.UNION_DECL,
    #CursorKind.STRUCT_DECL,
    CursorKind.ENUM_CONSTANT_DECL ]

def filter_node(node):
    if node is None:
        return False

    # # no node or no location
    # if node is None or node.location is None or node.location.file is None:
    #     return False

    # # not the main source file
    # if node.location.file.name != sys.argv[1]:
    #     return False

    if node.kind in CHILD_NODE_TYPES:
        return True

    return False

def get_struct_decl(node):
    for c in node.get_children():
        if c is not None and c.kind == CursorKind.STRUCT_DECL:
            return c
    return None

def node_children(node):

    if node is None:
        return []
    
    if isinstance(node,list):
        return node

    if node.kind == CursorKind.FIELD_DECL:
        if node.type.kind == TypeKind.ELABORATED:
            return node_children(node.type.get_declaration())
        elif node.type.kind == TypeKind.RECORD:
            return list(c for c in node.type.get_fields())
        elif node.type.kind == TypeKind.CONSTANTARRAY:
            et = node.type.element_type
            if et.kind == TypeKind.RECORD:
                return list(c for c in et.get_fields())
        elif node.type.kind == TypeKind.ENUM:
            return node_children(node.type.get_declaration())
        else:
            print("{} {}".format(str(node.type.kind),node.spelling))

    return list(c for c in node.get_children() if filter_node(c))

def get_array_size(f):
    et = f.type.element_type
    ec = f.type.element_count
    if et.spelling == 'char':
        return 'string({})'.format(ec)
    else:
        return '{} x {} bytes'.format(ec,et.get_size())

def get_field_size(f):
    if f.is_bitfield():
        return '{} bits'.format(f.get_bitfield_width())
    elif f.type.kind == TypeKind.CONSTANTARRAY:
        return get_array_size(f)
    return '{} bytes'.format(f.type.get_size())    

def print_field_decl(kind, text, f):
    ft = ''
    is_anon = f.is_anonymous()
    if f.type.kind == TypeKind.RECORD:
        decl = f.type.get_declaration()
        if decl.kind == CursorKind.UNION_DECL:
            if is_anon:
                ft = 'anonymous union ({})'.format(decl.mangled_name)
            else:
                ft = 'union {}'.format(f.type.spelling)
        elif decl.kind == CursorKind.STRUCT_DECL:
            if is_anon:
                ft = '{} anonymous struct'.format(f.kind)
            else:
                ft = 'struct {}'.format(f.type.spelling)
    elif f.type.kind == TypeKind.TYPEDEF:
        decl = f.type.get_canonical()
        ft = decl.spelling
    elif get_struct_decl(f) is not None:
        ft = 'anon struct'
    elif f.type.kind == TypeKind.CONSTANTARRAY:
        et = f.type.element_type
        ft = et.spelling
    else:
        ft = f.type.spelling

    return '{} {} ({})'.format(ft, text, get_field_size(f))

def print_enum_decl(kind, text, e):
    return '{} {} ({})'.format(kind, text, e.enum_value)

def print_union_decl(kind, text, f):
    return '{} {} ({})'.format(kind, text, get_field_size(f))

def print_node(node):

    if isinstance(node,list):
        return 'ROOT'

    text = node.spelling or node.displayname
    kind = str(node.kind)[str(node.kind).index('.')+1:]

    if CursorKind.FIELD_DECL == node.kind:
        return print_field_decl(kind, text, node)

    if CursorKind.ENUM_CONSTANT_DECL == node.kind:
        return print_enum_decl(kind, text, node)

    if CursorKind.UNION_DECL == node.kind:
        return print_union_decl(kind, text, node)
    
    return '{} {}'.format(kind, text)

def get_top_node(name, node):
    for c in node.get_children():
        if c.spelling == name:
            # struct found
            return c

    print("get_top_node: '{}' not found!".format(name))
    return None

def get_top_nodes(node):
    l = list()
    for c in node.get_children():
        # no node or no location
        if c is None or c.location is None or c.location.file is None:
            continue

        # not the main source file
        if c.location.file.name != sys.argv[1]:
            continue

        if c.kind in TOP_NODE_TYPES:
            # struct found
            l.append(c)

    return l

def print_top_node(name):
    s = get_top_node(name, translation_unit.cursor)
    if s is None:
        return
    print(asciitree.draw_tree(s, node_children, print_node))

def print_all():
    s = get_top_nodes(translation_unit.cursor)
    print(asciitree.draw_tree(s, node_children, print_node))


### Main ###

if len(sys.argv) < 3:
    print("Usage: dump_ast_yaml.py [header file name] [top node] [additional compile args]")
    sys.exit()

Config.set_library_file('/usr/local/Cellar/llvm/6.0.0/lib/libclang.dylib')
index = Index.create()
translation_unit = index.parse(sys.argv[1], ['-x', 'c++', '-std=c++11'] + sys.argv[3:])

print_top_node(sys.argv[2])
#print_top_node('CustomFunctionData')
#print_all()
