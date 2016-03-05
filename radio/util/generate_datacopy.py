#!/usr/bin/env python

import sys
import clang.cindex

structs = []

def build_struct(cursor):
    structs.append(cursor.spelling)
    print "template <class A, class B>\nvoid copy%s(A * dest, B * src)\n{" % cursor.spelling
    for c in cursor.get_children():
        if c.kind == clang.cindex.CursorKind.FIELD_DECL:
            if c.type.get_array_size() > 0:
                if c.type.get_array_element_type().spelling in structs:
                    print "  for (int i=0; i<%d; i++) {" % c.type.get_array_size()
                    print "    copy%s(&dest->%s[i], &src->%s[i]);" % (c.type.get_array_element_type().spelling, c.spelling, c.spelling)
                    print "  }"
                else:
                    print "  memcpy(&dest->%s, &src->%s, sizeof(dest->%s));" % (c.spelling, c.spelling, c.spelling)
            elif c.type.get_declaration().spelling in structs:
                print "  copy%s(&dest->%s, &src->%s);" % (c.type.get_declaration().spelling, c.spelling, c.spelling)
            else:
                print "  dest->%s = src->%s;" % (c.spelling, c.spelling)
    print "}\n"

def build(cursor):
    result = []
    for c in cursor.get_children():
        if c.kind == clang.cindex.CursorKind.STRUCT_DECL:
            build_struct(c)
    return result

index = clang.cindex.Index.create()
translation_unit = index.parse(sys.argv[1], ['-x', 'c++', '-std=c++11'] + sys.argv[2:])
build(translation_unit.cursor)
