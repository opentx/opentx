/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"
#include "storage.h"
#include "sdcard_common.h"

#include "modelslist.h"

#include "yaml/yaml_node.h"
#include "yaml/yaml_bits.h"

#include "yaml/yaml_datastructs_funcs.cpp"
#if defined(PCBX10)
#include "yaml/yaml_datastructs_x10.cpp"
#elif defined(PCBX12S)
#include "yaml/yaml_datastructs_x12s.cpp"
#else
#error "Board not supported by YAML storage"
#endif

const char * readYamlFile(const char* fullpath, const YamlParserCalls* calls, void* parser_ctx)
{
    FIL  file;
    UINT bytes_read;

    FRESULT result = f_open(&file, fullpath, FA_OPEN_EXISTING | FA_READ);
    if (result != FR_OK) {
        return SDCARD_ERROR(result);
    }
      
    YamlParser yp; //TODO: move to re-usable buffer
    yp.init(calls, parser_ctx);

    char buffer[32];
    while (f_read(&file, buffer, sizeof(buffer), &bytes_read) == FR_OK) {

      // reached EOF?
      if (bytes_read == 0)
        break;
      
      if (yp.parse(buffer, bytes_read) != YamlParser::CONTINUE_PARSING)
        break;
    }

    f_close(&file);
    return NULL;
}


//
// Generic storage interface
//

void storageCreateModelsList()
{
    modelslist.clear();
    ModelsCategory* cat = modelslist.createCategory(DEFAULT_CATEGORY, false);
    cat->addModel(DEFAULT_MODEL_FILENAME);
    modelslist.save();
}

//
// SDCARD storage interface
//

static const struct YamlNode radioDataNode = YAML_ROOT( struct_RadioData );

const char * loadRadioSettingsSettings()
{
    // YAML reader
    TRACE("YAML radio settings reader");

    YamlTreeWalker tree;
    tree.reset(&radioDataNode, (uint8_t*)&g_eeGeneral);

    return readYamlFile(RADIO_SETTINGS_YAML_PATH, &YamlTreeWalkerCalls, &tree);
}

struct yaml_writer_ctx {
    FIL*    file;
    FRESULT result;
};

static bool yaml_writer(void* opaque, const char* str, size_t len)
{
    UINT bytes_written;
    yaml_writer_ctx* ctx = (yaml_writer_ctx*)opaque;

    TRACE_NOCRLF("%.*s",len,str);

    ctx->result = f_write(ctx->file, str, len, &bytes_written);
    return (ctx->result == FR_OK) && (bytes_written == len);
}

const char * writeGeneralSettings()
{
    // YAML reader
    TRACE("YAML radio settings writer");

    FIL file;

    FRESULT result = f_open(&file, RADIO_SETTINGS_YAML_PATH, FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK) {
        return SDCARD_ERROR(result);
    }
      
    YamlTreeWalker tree;
    tree.reset(&radioDataNode, (uint8_t*)&g_eeGeneral);

    yaml_writer_ctx ctx;
    ctx.file = &file;
    ctx.result = FR_OK;
    
    if (!tree.generate(yaml_writer, &ctx)) {
        if (ctx.result != FR_OK) {
            f_close(&file);
            return SDCARD_ERROR(ctx.result);
        }
    }

    f_close(&file);
    return NULL;
}

static const struct YamlNode modelDataNode = YAML_ROOT( struct_ModelData );

const char * readModel(const char * filename, uint8_t * buffer, uint32_t size)
{
    // YAML reader
    TRACE("YAML model reader");

    char path[256];
    getModelPath(path, filename);

    YamlTreeWalker tree;
    tree.reset(&modelDataNode, buffer);

    return readYamlFile(path, &YamlTreeWalkerCalls, &tree);
}

const char * writeModel()
{
    // YAML reader
    TRACE("YAML model writer");

    char path[256];
    getModelPath(path, g_eeGeneral.currModelFilename);

    FIL file;

    FRESULT result = f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK) {
        return SDCARD_ERROR(result);
    }
      
    YamlTreeWalker tree;
    tree.reset(&modelDataNode, (uint8_t*)&g_model);

    yaml_writer_ctx ctx;
    ctx.file = &file;
    ctx.result = FR_OK;
    
    if (!tree.generate(yaml_writer, &ctx)) {
        if (ctx.result != FR_OK) {
            f_close(&file);
            return SDCARD_ERROR(ctx.result);
        }
    }

    f_close(&file);
    return NULL;
}
