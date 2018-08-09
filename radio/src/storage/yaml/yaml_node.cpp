#include "yaml_node.h"
#include "yaml_bits.h"
//#include "model.h"
//#include <stdio.h>
//#include <assert.h>
#include <string.h>

YamlTreeWalker::YamlTreeWalker()
    : stack_level(NODE_STACK_DEPTH),
      virt_level(0),
      anon_union(0)
{
    memset(stack,0,sizeof(stack));
}

void YamlTreeWalker::reset(const YamlNode* node)
{
    stack_level = NODE_STACK_DEPTH;
    virt_level  = 0;

    push();
    setNode(node);
    rewind();
}

bool YamlTreeWalker::push()
{
    if (full())
        return false;

    stack_level--;
    memset(&(stack[stack_level]), 0, sizeof(State));

    return true;
}

bool YamlTreeWalker::pop()
{
    if (empty())
        return false;

    memset(&(stack[stack_level]), 0, sizeof(State));
    stack_level++;
    return true;
}

// Rewind to the current node's first attribute
// (and reset the bit offset)
void YamlTreeWalker::rewind()
{
    if (getNode()->type == YDT_ARRAY
        || getNode()->type == YDT_UNION) {
        setAttrIdx(0);
        setAttrOfs(getLevelOfs());
    }
}

// Increment the cursor until a match is found or the end of
// the current collection (node of type YDT_NONE) is reached.
//
// return true if a match has been found.
bool YamlTreeWalker::findNode(const char* tag, uint8_t tag_len)
{
    if (virt_level)
        return false;
    
    rewind();

    const struct YamlNode* attr = getAttr();
    while(attr && attr->type != YDT_NONE) {

        if ((tag_len == attr->tag_len)
            && !strncmp(tag, attr->tag, tag_len)) {
            return true; // attribute found!
        }

        toNextAttr();
        attr = getAttr();
    }

    return false;
}

// Get the current bit offset
unsigned int YamlTreeWalker::getBitOffset()
{
    return stack[stack_level].getOfs();
}

bool YamlTreeWalker::toParent()
{
    if(virt_level) {
        virt_level--;
        return true;
    }

    if (!pop())
        return false;

    // const YamlNode* node = getNode();
    // if (!empty() && anon_union
    //     && (node->type == YDT_UNION) && (node->tag_len == 0)) {

    //     anon_union--;
    //     return toParent();
    // }

    return !empty();
}

bool YamlTreeWalker::toChild()
{
    const struct YamlNode* attr = getAttr();
    if (!attr
        || (attr->type != YDT_ARRAY
            && attr->type != YDT_UNION)) {
        virt_level++;
        return true;
    }

    if (!push()) {
        virt_level++;
        return false;
    }

    setNode(attr);
    setAttrOfs(getLevelOfs());
    return true;
}

bool YamlTreeWalker::toNextElmt()
{
    const struct YamlNode* node = getNode();
    if (!virt_level && (node->type == YDT_ARRAY
                        || node->type == YDT_UNION)) {

        if (node->type == YDT_UNION) {
            //printf("YDT_UNION: max-elmts: %u\n", node->u._array.u._a.elmts);
            return false;
        }

        if (getElmts() >= node->u._array.u._a.elmts - 1)
            return false;

        incElmts();
        rewind();
    }

    return true;
}

bool YamlTreeWalker::isElmtEmpty(uint8_t* data)
{
    if (virt_level)
        return true;

    if (!data)
        return false;
    
    const struct YamlNode* node = getNode();
    uint32_t bit_ofs = 0;

    if (node->type == YDT_ARRAY) {

        bit_ofs = ((uint32_t)getElmts())
            * ((uint32_t)getNode()->size)
            + getLevelOfs();

        // printf("ARRAY bit_ofs = %u (tag=%.*s;max-elmts=%u)",
        //        bit_ofs,node->tag_len,node->tag,node->u._array.u._a.elmts);
        // dump_stack();

        // assume structs aligned on 8bit boundaries
        if (node->u._array.u._a.is_active)
            return !node->u._array.u._a.is_active(data + (bit_ofs >> 3));

        return yaml_is_zero(data + (bit_ofs >> 3), node->size);
    }
    else if (node->type == YDT_UNION
             && stack_level < NODE_STACK_DEPTH - 1) {

        bit_ofs = getLevelOfs();

        // printf("UNION bit_ofs = %u\n", bit_ofs);

        return node->u._array.u.select_member;
            // // assume structs aligned on 8bit boundaries
            // && !node->_array.is_active(data + (bit_ofs >> 3));
    }

    return false;
}

void YamlTreeWalker::toNextAttr()
{
    const struct YamlNode* node = getNode();
    const struct YamlNode* attr = NULL;

    if (node->type != YDT_UNION) {
    
        attr = getAttr();
        unsigned int attr_bit_ofs = getAttrOfs();

        if (attr->type == YDT_ARRAY)
            attr_bit_ofs += ((uint32_t)attr->u._array.u._a.elmts * (uint32_t)attr->size);
        else
            attr_bit_ofs += (uint32_t)attr->size;

        setAttrOfs(attr_bit_ofs);
    }

    incAttr();

    // anonymous union handling
    attr = getAttr();
    if ((attr->type == YDT_UNION) && (attr->tag_len == 0)) {
        toChild();
        anon_union++;
    }
    else if ((attr->type == YDT_NONE)
             && (getNode()->type == YDT_UNION)
             && anon_union) {

        anon_union--;
        toParent();
        toNextAttr();
    }
}

void YamlTreeWalker::dump_stack()
{
    for (int i=0; i<NODE_STACK_DEPTH; i++) {
        const State& st = stack[i];
        //printf(" [%p|%u|%i|%i]",st.node,st.bit_ofs,st.attr_idx,st.elmts);
    }
    //printf("\n");
}
