#!/usr/bin/env python

import sys
import clang.cindex
import time
import os

if sys.platform == "darwin":
    if os.path.exists('/usr/local/Cellar/llvm/6.0.0/lib/libclang.dylib'):
        clang.cindex.Config.set_library_file('/usr/local/Cellar/llvm/6.0.0/lib/libclang.dylib')
    elif os.path.exists('/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/libclang.dylib'):
        clang.cindex.Config.set_library_file('/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/libclang.dylib')
    elif os.path.exists('/Library/Developer/CommandLineTools/usr/lib/libclang.dylib'):
        clang.cindex.Config.set_library_file('/Library/Developer/CommandLineTools/usr/lib/libclang.dylib')
elif sys.platform.startswith("linux"):
    for version in ("7", "6.0", "3.8"):
        libclang = "/usr/lib/x86_64-linux-gnu/libclang-%s.so.1" % version
        if os.path.exists(libclang):
            clang.cindex.Config.set_library_file(libclang)
            break

structs = []
extrastructs = []


def build_struct(cursor, anonymousUnion=False):
    if not anonymousUnion:
        structs.append(cursor.spelling)
        print("template <class A, class B>\nvoid copy%s(A * dest, B * src)\n{" % cursor.spelling)

    for c in cursor.get_children():
        if c.kind == clang.cindex.CursorKind.UNION_DECL:
            if c.spelling:
                raise Exception("Cannot handle non anonymous unions")

            copied_union_member = False
            for uc in c.get_children():
                if not uc.spelling or uc.kind == clang.cindex.CursorKind.PACKED_ATTR:
                    # Ignore
                    pass
                else:
                    # per default we copy only the first member of a union and warn if there are more
                    # members (declare the other members NOBACKUP)
                    if copied_union_member:
                        print("Warning more than one union member (%s) in anynomous union inside struct %s, consider NOBACKUP statements" % (uc.spelling, cursor.spelling), file=sys.stderr)
                    else:
                        copy_decl(uc, uc.spelling)
                        copied_union_member = True

        elif c.kind == clang.cindex.CursorKind.FIELD_DECL:
            copy_decl(c, c.spelling)

    if not anonymousUnion:
        print("}\n")


def build(cursor):
    result = []
    for c in cursor.get_children():
        if c.location.file.name == sys.argv[1]:
            if c.kind == clang.cindex.CursorKind.STRUCT_DECL:
                build_struct(c)
    for c, spelling in extrastructs:
        print("template <class A, class B>\nvoid copy%s(A * dest, B * src)\n{" % spelling)
        build_struct(c, True)
        print("}\n")

    return result


def copy_decl(c, spelling):
    children = [ch for ch in c.get_children()]
    if c.type.get_array_size() > 0:
        if c.type.get_array_element_type().spelling in structs:
            print("  for (int i=0; i<%d; i++) {" % c.type.get_array_size())
            print("    copy%s(&dest->%s[i], &src->%s[i]);" % (c.type.get_array_element_type().spelling, spelling, spelling))
            print("  }")
        else:
            print("  memcpy(dest->%s, src->%s, sizeof(dest->%s));" % (spelling, spelling, spelling))
    elif len(children) == 1 and children[0].kind == clang.cindex.CursorKind.STRUCT_DECL and not children[0].spelling:
        # inline declared structs
        if c.semantic_parent.spelling:
            spelling_func = c.semantic_parent.spelling + "_" + spelling
        else:
            spelling_func = c.semantic_parent.semantic_parent.spelling + "_" + spelling

        extrastructs.append((children[0], spelling_func))
        print("  copy%s(&dest->%s, &src->%s);" % (spelling_func, spelling, spelling))
    elif c.type.get_declaration().spelling in structs:
        print("  copy%s(&dest->%s, &src->%s);" % (c.type.get_declaration().spelling, spelling, spelling))
    else:
        print("  dest->%s = src->%s;" % (spelling, spelling))


def header():
    print("// This file was auto-generated by %s script on %s. Do not edit this file!\n\n\n" % (os.path.basename(sys.argv[0]), time.asctime()))


def print_translation_unit_diags(diags, prefix=''):
    for diag in diags:
        print(prefix + str(diag))
        print_translation_unit_diags(diag.children, '  ' + prefix)


def main():
    index = clang.cindex.Index.create()
    translation_unit = index.parse(sys.argv[1], ['-x', 'c++', '-std=c++11'] + sys.argv[2:])

    if translation_unit.diagnostics:
        print_translation_unit_diags(translation_unit.diagnostics)
        sys.exit(-1)


    header()
    build(translation_unit.cursor)


if __name__ == "__main__":
    main()
