#ifndef _node_h_
#define _node_h_

#include <stdint.h>
#include <stddef.h>

#define NODE_STACK_DEPTH 4

enum YamlDataType {
    YDT_NONE=0,
    YDT_IDX,
    YDT_SIGNED,
    YDT_UNSIGNED,
    YDT_STRING,
    YDT_ARRAY,
    YDT_ENUM,
    YDT_UNION,
    YDT_PADDING
};

struct YamlIdStr
{
    int          id;
    const char*  str;
};

struct YamlNode
{
    typedef bool (*is_active_func)(uint8_t* data);

    typedef uint32_t (*cust_to_uint_func)(const char* val, uint8_t val_len);

    // return false if error
    typedef bool (*writer_func)(void* opaque, const char* str, size_t len);

    typedef bool (*uint_to_cust_func)(uint32_t val, writer_func wf, void* opaque);

    typedef const struct YamlNode* (*select_member_func)(uint8_t* data);
    
    uint8_t      type;
    uint16_t     size;  // bits
    uint8_t      tag_len;
    const char*  tag;
    union {
        struct {
            const YamlNode* child;
            union {
                struct {
                    is_active_func  is_active;
                    uint16_t        elmts; // maximum number of elements
                } _a;
                select_member_func select_member;
            } u;
        } _array;

        struct {
            const YamlIdStr* choices;
        } _enum;

        struct {
            cust_to_uint_func cust_to_uint;
            uint_to_cust_func uint_to_cust;
        } _cust;
    } u;
};

#define YAML_TAG(str)                           \
    .tag_len=(sizeof(str)-1), .tag=(str)

#define YAML_IDX                                \
    { .type=YDT_IDX , .size=0, YAML_TAG("idx") }

#define YAML_SIGNED(tag, bits)                          \
    { .type=YDT_SIGNED, .size=(bits), YAML_TAG(tag), .u={._cust={ NULL, NULL }} }

#define YAML_UNSIGNED(tag, bits)                        \
    { .type=YDT_UNSIGNED, .size=(bits), YAML_TAG(tag), .u={._cust={ NULL, NULL }} }

#define YAML_SIGNED_CUST(tag, bits, f_cust_to_uint, f_uint_to_cust)     \
    { .type=YDT_SIGNED, .size=(bits), YAML_TAG(tag), .u={._cust={ .cust_to_uint=f_cust_to_uint, .uint_to_cust=f_uint_to_cust }} }

#define YAML_UNSIGNED_CUST(tag, bits, f_cust_to_uint, f_uint_to_cust)   \
    { .type=YDT_UNSIGNED, .size=(bits), YAML_TAG(tag), .u={._cust={ .cust_to_uint=f_cust_to_uint, .uint_to_cust=f_uint_to_cust }} }

#define YAML_STRING(tag, max_len)                               \
    { .type=YDT_STRING, .size=((max_len)<<3), YAML_TAG(tag) }

#define YAML_STRUCT(tag, bits, nodes, f_is_active)                     \
    { .type=YDT_ARRAY, .size=(bits), YAML_TAG(tag), .u={._array={ .child=(nodes), .u={ ._a={.is_active=(f_is_active), .elmts=1 }}}} }

#define YAML_ARRAY(tag, bits, max_elmts, nodes, f_is_active)           \
    { .type=YDT_ARRAY, .size=(bits), YAML_TAG(tag), .u={._array={ .child=(nodes), .u={ ._a={.is_active=(f_is_active), .elmts=(max_elmts) }}}} }

#define YAML_ENUM(tag, bits, id_strs)                                   \
    { .type=YDT_ENUM, .size=(bits), YAML_TAG(tag), .u={._enum={ .choices=(id_strs) }} }

#define YAML_UNION(tag, bits, nodes, f_sel_m)                       \
    { .type=YDT_UNION, .size=(bits), YAML_TAG(tag), .u={._array={ .child=(nodes), .u={.select_member=(f_sel_m) }}} }

#define YAML_PADDING(bits)                      \
    { .type=YDT_PADDING, .size=(bits) }

#define YAML_END                                \
    { .type=YDT_NONE, .size=0 }

#define YAML_ROOT(nodes)                                                \
    { .type=YDT_ARRAY, .size=0, .tag_len=0, .tag=NULL,                  \
            .u={                                                        \
            ._array={ .child=(nodes),                                   \
                      .u={._a={.is_active=NULL, .elmts=1 }}             \
            }}                                                          \
    }


class YamlTreeWalker
{
    struct State {
        const YamlNode* node;
        unsigned int    bit_ofs;
        uint8_t         attr_idx;
        uint8_t         elmts;

        inline unsigned int getOfs() {
            return bit_ofs + node->size * elmts;
        }
    };

    State   stack[NODE_STACK_DEPTH];
    uint8_t stack_level;
    uint8_t virt_level;
    uint8_t anon_union;

    unsigned int getAttrOfs() { return stack[stack_level].bit_ofs; }
    unsigned int getLevelOfs() {
        if (stack_level < NODE_STACK_DEPTH - 1) {
            return stack[stack_level + 1].getOfs();
        }
        return 0;
    }

    void setNode(const YamlNode* node) { stack[stack_level].node = node; }
    void setAttrIdx(uint8_t idx) { stack[stack_level].attr_idx = idx; }

    void setAttrOfs(unsigned int ofs) { stack[stack_level].bit_ofs = ofs; }

    void incAttr() { stack[stack_level].attr_idx++; }
    void incElmts() { stack[stack_level].elmts++; }

    bool empty() { return stack_level == NODE_STACK_DEPTH; }
    bool full()  { return stack_level == 0; }
    
    // return true on success
    bool push();
    bool pop();
    
public:
    YamlTreeWalker();

    void reset(const YamlNode* node);

    int getLevel() {
        return NODE_STACK_DEPTH - stack_level
            + virt_level - anon_union;
    }
    
    const YamlNode* getNode() {
        return stack[stack_level].node;
    }

    const YamlNode* getAttr() {
        uint8_t idx = stack[stack_level].attr_idx;
        return &(stack[stack_level].node->u._array.child[idx]);
    }

    unsigned int getElmts() {
        return stack[stack_level].elmts;
    }

    // Rewind to the current node's first attribute
    // (and reset the bit offset)
    void rewind();

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

    bool finished() { return empty(); }

    void dump_stack();
};


#endif
