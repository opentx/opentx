#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "yaml_parser.h"
#include "debug.h"

YamlParser::YamlParser()
{
}

void YamlParser::init(const YamlParserCalls* parser_calls, void* parser_ctx)
{
    indent = 0;
    memset(indents, 0, sizeof(indents));

    calls = parser_calls;
    ctx   = parser_ctx;
    reset();
}

void YamlParser::reset()
{
    state = ps_Indent;
    indents[calls->get_level(ctx) - 1] = indent;
    indent = scratch_len  = 0;
    node_found = false;
}

uint8_t YamlParser::getLastIndent()
{
    return indents[calls->get_level(ctx) - 1];
}

YamlParser::YamlResult
YamlParser::parse(const char* buffer, unsigned int size)
{

#define CONCAT_STR(s,s_len,c)                   \
    {                                           \
        if(s_len < MAX_STR)                     \
            s[s_len++] = c;                     \
        else {                                  \
            TRACE("STRING_OVERFLOW");           \
            return STRING_OVERFLOW;             \
        }                                       \
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
                    if (!calls->to_parent(ctx)) {
                        TRACE("STOP (no parent)!\n");
                        return DONE_PARSING;
                    }
                } while (indent < getLastIndent());

                if (state == ps_Dash) {
                    if (!calls->to_next_elmt(ctx)) {
                        return DONE_PARSING;
                    }
                }
            }
            // go down one level
            else if (indent > getLastIndent()) {
                if (!calls->to_child(ctx)) {
                    TRACE("STOP (stack full)!\n");
                    return DONE_PARSING; // ERROR
                }

                if (calls->get_level(ctx) > MAX_DEPTH) {
                    TRACE("STOP (indent stack full)!\n");
                    return DONE_PARSING; // ERROR
                }
            }
            // same level, next element
            else if (state == ps_Dash) {
                if (!calls->to_next_elmt(ctx)) {
                    return DONE_PARSING;
                }
            }

            state = ps_Attr;
            CONCAT_STR(scratch_buf, scratch_len, *c);
            break;

        case ps_Attr:
            if (*c == ' ') {// assumes nothing else comes after spaces start
                node_found = calls->find_node(ctx, scratch_buf, scratch_len);
                if (!node_found) {
                    TRACE("YAML_PARSER: Could not find node '%.*s' (1)\n",
                          scratch_len, scratch_buf);
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
                    node_found = calls->find_node(ctx, scratch_buf, scratch_len);
                    if (!node_found) {
                        TRACE("YAML_PARSER: Could not find node '%.*s' (2)\n",
                              scratch_len, scratch_buf);
                    }
                }
                state = ps_CRLF;
                continue;
            }
            if (*c == ':') {
                if (state == ps_Attr) {
                    node_found = calls->find_node(ctx, scratch_buf, scratch_len);
                    if (!node_found) {
                        TRACE("YAML_PARSER: Could not find node '%.*s' (3)\n",
                              scratch_len, scratch_buf);
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
                // WTF???: set attribute val=NULL
                // if (node_found) {
                //     calls->set_attr(ctx, NULL, 0);
                //     // yaml_set_attr(data, walker.getBitOffset(), walker.getAttr(), NULL, 0);
                //     // //walker.dump_stack();
                // }
                state = ps_CRLF;
                continue;
            }
            state = ps_Val;
            scratch_len = 0;
            if (*c == '\"') {
                state = ps_ValQuo;
                break;
            }
            CONCAT_STR(scratch_buf, scratch_len, *c);
            break;

        case ps_ValQuo:
            if (*c == '\"') {
                state = ps_Val;
                break;
            }
            if (*c == '\\') {
                state = ps_ValEsc1;
                break;
            }
            CONCAT_STR(scratch_buf, scratch_len, *c);
            break;

        case ps_ValEsc1:
            if (*c == 'x') {
                state = ps_ValEsc2;
                break;
            }
            //TODO: more escapes needed???
            TRACE("unknown escape char '%c'",*c);
            return DONE_PARSING;

        case ps_ValEsc2:
            if(scratch_len >= MAX_STR) {
                TRACE("STRING_OVERFLOW");
                return STRING_OVERFLOW;
            }
            else if (*c >= '0' && *c <= '9') {
                scratch_buf[scratch_len] = (*c - '0') << 4;
                state = ps_ValEsc3;
                break;
            }
            else if (*c >= 'A' && *c <= 'F') {
                scratch_buf[scratch_len] = (*c - 'A' + 10) << 4;
                state = ps_ValEsc3;
                break;
            }
            TRACE("wrong hex digit '%c'",*c);
            return DONE_PARSING;

        case ps_ValEsc3:
            if (*c >= '0' && *c <= '9') {
                scratch_buf[scratch_len++] |= (*c - '0');
                state = ps_ValQuo;
                break;
            }
            else if (*c >= 'A' && *c <= 'F') {
                scratch_buf[scratch_len++] |= (*c - 'A' + 10);
                state = ps_ValQuo;
                break;
            }
            TRACE("wrong hex digit '%c'",*c);
            return DONE_PARSING;
            
        case ps_Val:
            if (*c == ' ' || *c == '\r' || *c == '\n') {
                // set attribute
                if (node_found) {
                    calls->set_attr(ctx, scratch_buf, scratch_len);
                    // if (walker.getAttr()->type == YDT_IDX) {
                    //     uint32_t i = str2uint(scratch_buf, scratch_len);
                    //     while ((i > walker.getElmts()) && walker.toNextElmt());
                    // }
                    // else {
                    //     yaml_set_attr(data, walker.getBitOffset(), walker.getAttr(),
                    //                   scratch_buf, scratch_len);
                    //     //walker.dump_stack();
                    // }
                }
                state = ps_CRLF;
                continue;
            }
            if (*c == '\"') {
                state = ps_ValQuo;
                break;
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

