#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "yaml_parser.h"
#include "yaml_node.h"
#include "yaml_bits.h"

#define MIN(a,b) (a < b ? a : b)

static int str2int(const char* val, uint8_t val_len)
{
    bool  neg = false;
    int i_val = 0;
    
    for(uint8_t i=0; i < val_len; i++) {
        if (val[i] == '-')
            neg = true;
        else if (val[i] >= '0' && val[i] <= '9') {
            i_val = i_val * 10 + (val[i] - '0');
        }
    }

    return neg ? -i_val : i_val;
}

static uint32_t str2uint(const char* val, uint8_t val_len)
{
    uint32_t i_val = 0;
    
    for(uint8_t i=0; i < val_len; i++) {
        if (val[i] >= '0' && val[i] <= '9') {
            i_val = i_val * 10 + (val[i] - '0');
        }
    }

    return i_val;
}

static void copy_string(char* dst, const char* src, uint8_t len)
{
    memcpy(dst,src,len);
    dst[len] = '\0';
}

static uint32_t parse_enum(const struct YamlIdStr* choices, const char* val, uint8_t val_len)
{
    while (choices->str) {

        // we have a match!
        if (!strncmp(val, choices->str, val_len))
            break;

        choices++;
    }

    return choices->id;
}

static void yaml_set_attr(uint8_t* ptr, uint32_t bit_ofs, const YamlNode* node,
                          const char* val, uint8_t val_len)
{
    uint32_t i = 0;

    // printf("set(%s, %.*s, bit-ofs=%u, bits=%u)\n",
    //        node->tag, val_len, val, bit_ofs, node->size);

    ptr += bit_ofs >> 3UL;
    bit_ofs &= 0x07;

    if (node->type == YDT_STRING) {
        assert(!bit_ofs);
        copy_string((char*)ptr, val, MIN(val_len, node->size - 1));
        return;
    }

    switch(node->type) {
    case YDT_SIGNED:
        i = node->u._cust.cust_to_uint ? node->u._cust.cust_to_uint(val, val_len)
            : (uint32_t)str2int(val, val_len);
        break;
    case YDT_UNSIGNED:
        i = node->u._cust.cust_to_uint ? node->u._cust.cust_to_uint(val, val_len)
            : str2uint(val, val_len);
        break;
    case YDT_ENUM:
        i = parse_enum(node->u._enum.choices, val, val_len);
        break;
    default:
        break;
    }

    yaml_put_bits(ptr, i, bit_ofs, node->size);
}

static char int2str_buffer[MAX_STR] = {0};
static const char _int2str_lookup[] = { '0', '1', '2', '3', '4', '5', '6' , '7', '8', '9' };

char* unsigned2str(uint32_t i)
{
    char* c = &(int2str_buffer[MAX_STR-2]);
    do {
        *(c--) = _int2str_lookup[i % 10];
        i = i / 10;
    } while((c > int2str_buffer) && i);

    return (c + 1);
}

char* signed2str(int32_t i)
{
    if (i < 0) {
        char* c = unsigned2str(-i);
        *(--c) = '-';
        return c;
    }

    return unsigned2str((unsigned int)i);
}

static int32_t to_signed(uint32_t i, uint8_t bits)
{
    if (i & (1 << (bits-1))) {
        i |= 0xFFFFFFFF << bits;
    }

    return i;
}

static const char* yaml_output_enum(uint32_t i, const struct YamlIdStr* choices)
{
    while(choices->str) {
        if (i == choices->id)
            break;
        choices++;
    }

    return choices->str;
}

static bool yaml_output_attr(uint8_t* ptr, uint32_t bit_ofs, const YamlNode* node,
                             YamlNode::writer_func wf, void* opaque)
{
    if (node->type == YDT_NONE)
        return false;
    
    if (node->type == YDT_PADDING)
        return true;
    
    // output tag
    if (!wf(opaque, node->tag, node->tag_len))
        return false;

    if (!wf(opaque, ": ", 2))
        return false;

    if (ptr) {
    
        ptr += bit_ofs >> 3UL;
        bit_ofs &= 0x07;

        const char* p_out = NULL;
        if (node->type == YDT_STRING) {
            assert(!bit_ofs);
            p_out = (const char*)ptr;
        }
        else {
            unsigned int i = yaml_get_bits(ptr, bit_ofs, node->size);

            if ((node->type == YDT_SIGNED || node->type == YDT_UNSIGNED)
                && node->u._cust.uint_to_cust) {
                return node->u._cust.uint_to_cust(i, wf, opaque);
            }
            else {
                switch(node->type) {
                case YDT_SIGNED:
                    p_out = signed2str((int)to_signed(i, node->size));
                    break;
                case YDT_UNSIGNED:
                    p_out = unsigned2str(i);
                    break;
                case YDT_ENUM:
                    p_out = yaml_output_enum(i, node->u._enum.choices);
                    break;

                case YDT_ARRAY:
                case YDT_UNION:
                default:
                    break;
                }
            }
        }

        if (p_out && !wf(opaque, p_out, strlen(p_out)))
            return false;
    }

    if (!wf(opaque, "\r\n", 2))
        return false;

    return true;
}

YamlParser::YamlParser()
{
}

void YamlParser::init(const YamlNode * node)
{
    indent = 0;
    memset(indents, 0, sizeof(indents));
    walker.reset(node);
    reset();
}

void YamlParser::reset()
{
    state = ps_Indent;
    indents[walker.getLevel() - 1] = indent;
    indent = scratch_len  = 0;
    node_found = false;
}

uint8_t YamlParser::getLastIndent()
{
    return indents[walker.getLevel() - 1];
}

YamlParser::YamlResult
YamlParser::parse(const char* buffer, unsigned int size, uint8_t* data)
{

#define CONCAT_STR(s,s_len,c)                   \
    {                                           \
        if(s_len < MAX_STR)                     \
            s[s_len++] = c;                     \
        else                                    \
            return STRING_OVERFLOW;             \
    }

    const char* c   = buffer;
    const char* end = c + size;

    while(c < end) {

        switch(state) {

        case ps_Indent:
            if (*c == '-') {
                state = ps_Dash;
                ++indent;
                break;
            }
            // trap
        case ps_Dash:
            if (*c == ' ') { // skip space(s), should be only one??
                ++indent;
                break;
            }

            if (indent < getLastIndent()) {
                // go up as many levels as necessary
                do {
                    if (!walker.toParent()) {
                        //printf("STOP (no parent)!\n");
                        return DONE_PARSING;
                    }
                } while (indent < getLastIndent());

                if (state == ps_Dash) {
                    if (!walker.toNextElmt()) {
                        return DONE_PARSING;
                    }
                }
            }
            // go down one level
            else if (indent > getLastIndent()) {
                if (!walker.toChild()) {
                    //printf("STOP (stack full)!\n");
                    return DONE_PARSING; // ERROR
                }

                if (walker.getLevel() > MAX_DEPTH) {
                    //printf("STOP (indent stack full)!\n");
                    return DONE_PARSING; // ERROR
                }
            }
            // same level, next element
            else if (state == ps_Dash) {
                if (!walker.toNextElmt()) {
                    return DONE_PARSING;
                }
            }

            state = ps_Attr;
            CONCAT_STR(scratch_buf, scratch_len, *c);
            break;

        case ps_Attr:
            if (*c == ' ') {// assumes nothing else comes after spaces start
                node_found = walker.findNode(scratch_buf, scratch_len);
                if (!node_found) {
                    //printf("Could not find node '%.*s' (1)\n", scratch_len, scratch_buf);
                }
                state = ps_AttrSP;
                break;
            }
            if (*c != ':')
                CONCAT_STR(scratch_buf, scratch_len, *c);
            // trap
        case ps_AttrSP:
            if (*c == '\r' || *c == '\n') {
                if (state == ps_Attr) {
                    node_found = walker.findNode(scratch_buf, scratch_len);
                    if (!node_found) {
                        //printf("Could not find node '%.*s' (2)\n", scratch_len, scratch_buf);
                    }
                }
                state = ps_CRLF;
                continue;
            }
            if (*c == ':') {
                if (state == ps_Attr) {
                    node_found = walker.findNode(scratch_buf, scratch_len);
                    if (!node_found) {
                        //printf("Could not find node '%.*s' (3)\n", scratch_len, scratch_buf);
                    }
                }
                state = ps_Sep;
                break;
            }
            break;

        case ps_Sep:
            if (*c == ' ')
                break;
            if (*c == '\r' || *c == '\n'){
                // set attribute val=NULL
                if (node_found) {
                    yaml_set_attr(data, walker.getBitOffset(), walker.getAttr(), NULL, 0);
                    //walker.dump_stack();
                }
                state = ps_CRLF;
                continue;
            }
            state = ps_Val;
            scratch_len = 0;
            CONCAT_STR(scratch_buf, scratch_len, *c);
            break;

        case ps_Val:
            if (*c == ' ' || *c == '\r' || *c == '\n') {
                // set attribute
                if (walker.getAttr()->type == YDT_IDX) {
                    uint32_t i = str2uint(scratch_buf, scratch_len);
                    while ((i > walker.getElmts()) && walker.toNextElmt());
                }
                else {
                    if (node_found) {
                        yaml_set_attr(data, walker.getBitOffset(), walker.getAttr(),
                                      scratch_buf, scratch_len);
                        //walker.dump_stack();
                    }
                }
                state = ps_CRLF;
                continue;
            }
            CONCAT_STR(scratch_buf, scratch_len, *c);
            break;
                
        case ps_CRLF:
            if (*c == '\n') {
                // reset state at EOL
                reset();
            }
            break;
        }

        c++;
    } // for each char

    return CONTINUE_PARSING;
}

bool YamlParser::generate(uint8_t* data, YamlNode::writer_func wf, void* opaque)
{
    bool new_elmt = false;
    
    while (true) {
        const struct YamlNode* attr = walker.getAttr();

        if (attr->type == YDT_PADDING) {
            walker.toNextAttr();
            continue;
        }
        
        // end of this level, go up or die
        if (attr->type == YDT_NONE) {

            const struct YamlNode* node = walker.getNode();
            if (node->type != YDT_ARRAY && node->type != YDT_UNION)
                return false; // Error in the structure (probably)

            // walk to next non-empty element
            while (walker.toNextElmt()) {
                if (!walker.isElmtEmpty(data)) {
                    new_elmt = true;
                    break;
                }
            }

            if (new_elmt)
                continue;

            // no next element, go up
            if (!walker.toParent()) {

                return true;
            }

            walker.toNextAttr();
            continue;
        }
        else if (attr->type == YDT_ARRAY || attr->type == YDT_UNION) {

            if (!walker.toChild())
                return false; // TODO: error handling???

            // walk to next non-empty element
            do {
                if (!walker.isElmtEmpty(data)) {
                    new_elmt = true;
                    break;
                }
                new_elmt = false;
            } while (walker.toNextElmt());

            if (new_elmt) {
                // non-empty element present in a new structure/array
                // let's output the attribute
                for(int i=2; i < walker.getLevel(); i++)
                    if (!wf(opaque, "   ", 3))
                        return false;
                if (!yaml_output_attr(NULL, 0, walker.getNode(), wf, opaque))
                    return false; // TODO: error handling???
                continue;
            }

            // no next element, go up
            if (!walker.toParent())
                return true;

            walker.toNextAttr();
            continue;
        }

        if (new_elmt) {

            for(int i=2; i < walker.getLevel(); i++)
                if (!wf(opaque, "   ", 3))
                    return false;

            if (!wf(opaque, " - ", 3))
                return false;

            new_elmt = false;
        }
        else {
            for(int i=1; i < walker.getLevel(); i++)
                if (!wf(opaque, "   ", 3))
                    return false;
        }

        if (attr->type == YDT_IDX) {

            if (!wf(opaque, "idx: ", 5))
                return false;

            char* idx = unsigned2str(walker.getElmts());
            if (!wf(opaque, idx, strlen(idx)))
                return false;

            if (!wf(opaque, "\r\n", 2))
                return false;
        }
        else if (!yaml_output_attr(data, walker.getBitOffset(), attr, wf, opaque))
            return false; // TODO: error handling???

        walker.toNextAttr();
    }

    return true;
}
