#include "opentx.h"
#include "yaml_bits.h"

#define GVAR_SMALL 128

static uint32_t in_read_weight(const YamlNode* node, const char* val, uint8_t val_len)
{
    if ((val_len == 4)
        && (val[0] == '-')
        && (val[1] == 'G')
        && (val[2] == 'V')
        && (val[3] >= '1')
        && (val[3] <= '9')) {

        TRACE("%.*s -> %i\n", val_len, val, GVAR_SMALL - (val[3] - '0'));
        return GVAR_SMALL - (val[3] - '0'); // -GVx => 128 - x
    }

    if ((val_len == 3)
        && (val[0] == 'G')
        && (val[1] == 'V')
        && (val[2] >= '1')
        && (val[2] <= '9')) {

        TRACE("%.*s -> %i\n", val_len, val, -GVAR_SMALL + (val[2] - '1'));
        return -GVAR_SMALL + (val[2] - '1'); //  GVx => -128 + (x-1)
    }

    return (uint32_t)yaml_str2int(val, val_len);
}

static bool in_write_weight(const YamlNode* node, uint32_t val, yaml_writer_func wf, void* opaque)
{
    int32_t sval = yaml_to_signed(val,node->size);
    
    if (sval > GVAR_SMALL-11 && sval < GVAR_SMALL-1) {
        char n = GVAR_SMALL - sval + '0';
        if (!wf(opaque, "-GV", 3)
            || !wf(opaque, &n, 1)
            || !wf(opaque, "\r\n", 2))
            return false;
        return true;
    }
    else if (sval < -GVAR_SMALL+11 && sval > -GVAR_SMALL+1) {
        char n = val - GVAR_SMALL + '1';
        if (!wf(opaque, "GV", 2)
            || !wf(opaque, &n, 1)
            || !wf(opaque, "\r\n", 2))
            return false;
        return true;
    }

    char* s = yaml_signed2str(sval);
    if (!wf(opaque, s, strlen(s))
        || !wf(opaque, "\r\n", 2))
        return false;
    return true;
}

static uint8_t select_zov(uint8_t* data)
{
    data -= sizeof(ZoneOptionValueEnum);
    ZoneOptionValueEnum* p_zovt = (ZoneOptionValueEnum*)data;
    return *p_zovt;
}

static uint8_t select_mod_type(uint8_t* data)
{
    //TODO: check ModuleData::type
    return 0;
}

static uint8_t select_custom_fn(uint8_t* data)
{
    // always use 'all'
    return 1;
}

static uint8_t select_script_input(uint8_t* data)
{
    // always use 'value'
    return 0;
}

static uint8_t select_id1(uint8_t* data)
{
    // always use 'id'
    return 0;
}

static uint8_t select_id2(uint8_t* data)
{
    // always use 'instance'
    return 0;
}

static uint8_t select_sensor_cfg(uint8_t* data)
{
    // always use 'param'
    return 5;
}
