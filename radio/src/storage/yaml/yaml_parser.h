#ifndef _yaml_parser_h_
#define _yaml_parser_h_

#include <stdint.h>

#define MAX_STR 40
#define MAX_DEPTH 16 // 12 real + 4 virtual

struct YamlParserCalls
{
    bool (*to_parent)    (void* ctx);
    bool (*to_child)     (void* ctx);
    bool (*to_next_elmt) (void* ctx);
    int  (*get_level)    (void* ctx);
    bool (*find_node)    (void* ctx, char* buf, uint8_t len);
    void (*set_attr)     (void* ctx, char* buf, uint8_t len);
};

class YamlParser
{
    enum ParserState {
        ps_Indent=0,
        ps_Dash,
        ps_Attr,
        ps_AttrSP,
        ps_Sep,
        ps_Val,
        ps_ValQuo,
        ps_ValEsc1,
        ps_ValEsc2,
        ps_ValEsc3,
        ps_CRLF
    };

    // last indents for each level
    uint8_t indents[MAX_DEPTH];

    // current indent
    uint8_t indent;

    // parser state
    uint8_t state;

    // scratch buffer w/ 16 bytes
    // used for attribute and values
    char    scratch_buf[MAX_STR];
    uint8_t scratch_len;

    bool node_found;

    // tree iterator state
    const YamlParserCalls* calls;
    void*                  ctx;
    
    // Reset parser state for next line
    void reset();

    uint8_t getLastIndent();

public:

    enum YamlResult {
        DONE_PARSING,
        CONTINUE_PARSING,
        STRING_OVERFLOW
    };

    YamlParser();

    void init(const YamlParserCalls* parser_calls, void* parser_ctx);
    
    YamlResult parse(const char* buffer, unsigned int size);
};

#endif
