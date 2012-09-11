#include "sam-gui/common/sam_gui_color.h"
#include "sam-gui/common/sam_gui_errors.h"
#include <stdlib.h>

extern int COLOR_GetGradientValue( SGUIColor* pclrBegin, SGUIColor* pclrEnd,
                                   uint32_t dwNumber, uint32_t dwIndex, SGUIColor* pclrResult )
{
    // Check pointers
    if ( (pclrBegin == NULL) || (pclrEnd == NULL) || (pclrResult == NULL) )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    // Check parameters consistency
    if ( dwNumber == 0 )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    // Process gradient

    return SAMGUI_E_OK ;
}

extern int COLOR_RGB565( SGUIColor* pclrIn, SGUIColor* pclrResult )
{
    // Check pointers
    if ( (pclrIn == NULL) || (pclrResult == NULL) )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    pclrResult->u.RGBA.ucR=pclrIn->u.RGBA.ucR&0x1f ;
    pclrResult->u.RGBA.ucG=pclrIn->u.RGBA.ucG&0x3f ;
    pclrResult->u.RGBA.ucB=pclrIn->u.RGBA.ucB&0x1f ;
    pclrResult->u.RGBA.ucA=0x00 ;

    return SAMGUI_E_OK ;
}
