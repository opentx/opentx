#!/usr/bin/python
import sys
from clang.cindex import *

# debug
import asciitree # must be version 0.2

# output
import jinja2

def print_error(*args):
    print("ERROR:",*args,file=sys.stderr)

def bail_out(*args):
    print_error(*args)
    sys.exit(-1)

_anon_cnt = 0

def get_next_anon():
    global _anon_cnt
    label = str(_anon_cnt)
    _anon_cnt = _anon_cnt + 1
    return label

SIGNED_TYPES  = ['char', 'short', 'int', 'long' ]

def map_type(type_name):
    if 'unsigned' in type_name:
        return 'unsigned'
    elif ('signed' in type_name) or (type_name in SIGNED_TYPES):
        return 'signed'

    return type_name

class AST_Element:
    def __init__(self, name, cursor):
        self.name   = name
        self.cursor = cursor
        self.elmts = []

    def get_children(self):
        return self.cursor.get_children()

    def get_elmts(self):
        return self.elmts

    def append(self,elmt):
        self.elmts.append(elmt)

    def str(self):
        if hasattr(self,'value'):
            if hasattr(self,'ann'):
                return '{}:\t{} ({})'.format(self.name,self.value,self.ann)
            return '{}:\t{}'.format(self.name,self.value)
        elif hasattr(self,'type'):
            return '{}\t{}'.format(self.type, self.name)
        return self.name

class FieldAST(AST_Element):
    def __init__(self, name, cursor):
        super(FieldAST, self).__init__(name, cursor)

        self.is_array = False
        
        if cursor.type.kind == TypeKind.TYPEDEF:
            self.type = map_type(cursor.type.get_canonical().spelling)
        elif cursor.type.kind == TypeKind.CONSTANTARRAY:
            self.type = cursor.type.element_type.spelling
            if self.type == 'char':
                self.type   = 'string'
                self.length = cursor.type.element_count
            self.is_array = True
        else:
            self.type = cursor.type.spelling

        if cursor.is_bitfield():
            self.bits = cursor.get_bitfield_width()
        else:
            self.bits = cursor.type.get_size() * 8


class StructAST(AST_Element):
    def __init__(self, name, cursor):
        if name == '':
            name = 'anonymous_' + self.name_prefix() + get_next_anon()
        else:
            name = self.name_prefix() + name
        super(StructAST, self).__init__(name, cursor)

    def name_prefix(self):
        return 'struct_'

class UnionAST(StructAST):
    def name_prefix(self):
        return 'union_'
        
class EnumAST(AST_Element):
    def __init__(self, name, cursor):
        super(EnumAST, self).__init__('enum_' + name, cursor)

class AST:
    def __init__(self):
        self.structs = []
        self.enums = []

    def has_enum(self,name):
        for e in self.enums:
            if e.name == name:
                return True
        return False
        
    def get_enums(self):
        return self.enums

    def get_structs(self):
        return self.structs

    def get_elmts(self):
        return self.enums + self.structs
    
    def append(self, elmt):
        if isinstance(elmt, StructAST):
            #print(elmt)
            self.structs.append(elmt)
        elif isinstance(elmt, EnumAST):
            self.enums.append(elmt)

    def str(self):
        return 'ROOT'

def parse_struct(node):
    st = StructAST(node.spelling, node)
    for c in node.get_children():
        parse_field(st,c)            
    return st

def parse_union(node):
    st = UnionAST(node.spelling, node)
    for c in node.get_children():
        parse_field(st,c)
    return st

def get_annotations(node):
    l = list()
    for c in node.get_children():
        if c.kind == CursorKind.ANNOTATE_ATTR:
            ann = c.displayname.split(':')
            l.append({'type':ann[0], 'val':ann[1]})
    return l

def parse_field(ast,node):

    if (not node.is_anonymous()) and (node.kind != CursorKind.FIELD_DECL):
        return

    #print('{} {}'.format(node.spelling, str(node.kind)))

    st = None
    f = FieldAST(node.spelling, node)

    if node.type.kind == TypeKind.RECORD:
        st = parse_node(RootAST,node.type.get_declaration())
    elif node.type.kind == TypeKind.ELABORATED:
        st = parse_node(RootAST,node.type.get_declaration())
    elif node.type.kind == TypeKind.CONSTANTARRAY:
        et = node.type.element_type
        st = parse_node(RootAST,et.get_declaration())
    elif node.type.kind == TypeKind.ENUM:
        st = parse_node(RootAST,node.type.get_declaration())
    else:
        pass
        #print('{} {}'.format(node.spelling, str(node.type.kind)))

    ann = get_annotations(node)
    if len(ann) > 0:
        for a in ann:
            #print(RootAST.get_enums())
            if a['type'] == 'enum' and not RootAST.has_enum(a['val']):
                parse_node(RootAST,get_top_node(a['val']))
    elif st is not None:
        f.type = st.name

    ast.append(f)


def parse_enum_field(ast,node):
    st = AST_Element(node.spelling[node.spelling.index('_')+1:], node)
    st.value = node.spelling #node.enum_value
    ast.append(st)
    # debug
    ann = get_annotations(node)
    if len(ann) > 0:
        #print(ann)
        st.ann = ann

def parse_enum(ast,node):
    st = EnumAST(node.spelling or node.displayname, node)
    for c in node.get_children():
        parse_enum_field(st,c)
    return st
        
def parse_node(ast,node):
    st = None

    if node.kind == CursorKind.STRUCT_DECL:
        st = parse_struct(node)
    elif node.kind == CursorKind.UNION_DECL:
        st = parse_union(node)
    elif node.kind == CursorKind.FIELD_DECL:
        parse_field(ast,node)
    elif node.kind == CursorKind.ENUM_DECL:
        st = parse_enum(ast,node)
    elif node.kind in [CursorKind.NO_DECL_FOUND, CursorKind.TYPEDEF_DECL]:
        pass
    else:
        pass
        #print('{} {}'.format(str(node.kind),node.spelling))

    if st is not None:
        ast.append(st)

    return st

### main ###

def get_top_node(name):
    node = translation_unit.cursor
    for c in node.get_children():
        if c.spelling == name:
            # struct found
            return c

    bail_out(f"'{name}' not found")

def ast_children(ast_node):
    if ast_node is not None:
        return ast_node.get_elmts()
    return []

def print_ast_node(ast_node):
    #if isinstance(ast_node,dict):
    #    return str(ast_node)
    return ast_node.str()

if len(sys.argv) < 2:
    bail_out(f"usage: {sys.argv[0]} [header file name] [template] [node name] [additional compile args]")

Config.set_library_file('/usr/local/Cellar/llvm/6.0.0/lib/libclang.dylib')
index = Index.create()
translation_unit = index.parse(sys.argv[1], ['-x', 'c++', '-std=c++11'] + sys.argv[4:])

RootAST = AST()
parse_node(RootAST, get_top_node(sys.argv[3]))

#print("Enums:", RootAST.get_enums())
#print("Structs:", RootAST.get_structs())
print(asciitree.draw_tree(RootAST, ast_children, print_ast_node))

#template = jinja2.Template(open(sys.argv[2]).read(), lstrip_blocks=True, trim_blocks=True)
#print(template.render(root=RootAST))
