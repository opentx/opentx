#ifndef _YAML_DEFS_H_
#define _YAML_DEFS_H_

#if defined(YAML_GENERATOR)

/* private definitions */
#define _yaml_note(label) #label
#define _yaml_attribute(attr) __attribute__((annotate(attr)))

/* public definitions */
#define ENUM(label) _yaml_attribute("enum:" _yaml_note(label))
#define SKIP        _yaml_attribute("skip:true")
#define USE_IDX     _yaml_attribute("idx:true")
#define FUNC(name)  _yaml_attribute("func:" _yaml_note(name))
#define NAME(label) _yaml_attribute("name:" _yaml_note(label))

#define CUST(read,write)                        \
  _yaml_attribute("read:" _yaml_note(read))     \
  _yaml_attribute("write:" _yaml_note(write))

#define ARRAY(elmt_size,elmt_type,fcn)    \
    _yaml_attribute("array:" _yaml_note(elmt_size) "|" \
                    _yaml_note(elmt_type) "|" _yaml_note(fcn))

#else

#define ENUM(label)
#define SKIP
#define USE_IDX
#define FUNC(name)
#define NAME(label)
#define CUST(read,write)
#define ARRAY(elmt_size,elmt_type,fcn)

#endif

#endif
