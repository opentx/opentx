#include "sam-gui/common/sam_gui_font.h"
#include "sam-gui/common/sam_gui_errors.h"
#include <stdlib.h>

const SGUIFont g_Font10x14=
{
    .dwType=WGT_FONT_TYPE_BITMAP,
    .dwWidth=10 ,
    .dwHeight=14 ,
    .pvData=(void*)aucFont10x14
} ;