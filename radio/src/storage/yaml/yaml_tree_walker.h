#ifndef _YAML_TREE_WALKER_H_
#define _YAML_TREE_WALKER_H_

#include <stdint.h>
#include "yaml_node.h"

struct YamlParserCalls;

class YamlTreeWalker
{
    struct State {
        const YamlNode* node;
        uint32_t    bit_ofs;
        int8_t      attr_idx;
        uint16_t    elmts;

        inline uint32_t getOfs() {
            return bit_ofs + node->size * elmts;
        }
    };

    State   stack[NODE_STACK_DEPTH];
    uint8_t stack_level;
    uint8_t virt_level;
    uint8_t anon_union;

    uint8_t* data;

    uint32_t getAttrOfs() { return stack[stack_level].bit_ofs; }
    uint32_t getLevelOfs() {
        if (hasParent()) {
            return stack[stack_level + 1].getOfs();
        }
        return 0;
    }

    const YamlNode* getParent() {
        if (hasParent())
            return stack[stack_level + 1].node;

        return nullptr;
    }
    
    void setNode(const YamlNode* node) { stack[stack_level].node = node; }
    void setAttrIdx(uint8_t idx) { stack[stack_level].attr_idx = idx; }

    void setAttrOfs(unsigned int ofs) { stack[stack_level].bit_ofs = ofs; }

    void incAttr() { stack[stack_level].attr_idx++; }
    void incElmts() { stack[stack_level].elmts++; }

    bool empty() { return stack_level == NODE_STACK_DEPTH; }
    bool full()  { return stack_level == 0; }

    bool hasParent() { return stack_level < NODE_STACK_DEPTH -1; }
    
    // return true on success
    bool push();
    bool pop();
    
    // Rewind to the current node's first attribute
    // (and reset the bit offset)
    void rewind();

public:
    YamlTreeWalker();

    void reset(const YamlNode* node, uint8_t* data);

    int getLevel() {
        return NODE_STACK_DEPTH - stack_level
            + virt_level - anon_union;
    }
    
    const YamlNode* getNode() {
        return stack[stack_level].node;
    }

    const YamlNode* getAttr() {
        int8_t idx = stack[stack_level].attr_idx;
        if (idx >= 0)
            return &(stack[stack_level].node->u._array.child[idx]);

        return NULL;
    }

    uint16_t getElmts() {
        return stack[stack_level].elmts;
    }

    // Increment the cursor until a match is found or the end of
    // the current collection (node of type YDT_NONE) is reached.
    //
    // return true if a match has been found.
    bool findNode(const char* tag, uint8_t tag_len);

    // Get the current bit offset
    unsigned int getBitOffset();

    bool toParent();
    bool toChild();

    bool toNextElmt();
    void toNextAttr();

    bool isElmtEmpty(uint8_t* data);

    void setAttrValue(char* buf, uint8_t len);

    bool generate(yaml_writer_func wf, void* opaque);

    void dump_stack();

    static const YamlParserCalls* get_parser_calls();
};

#endif
