#include "sam-gui/common/sam_gui_bitmap_raw.h"
#include "sam-gui/common/sam_gui_errors.h"
#include <stdlib.h>
#include <string.h>

extern int BitmapRaw_Initialize( SGUIBitmapRaw* pbmpBitmap, uint32_t dwWidth, uint32_t dwHeight, uint32_t dwBPP )
{
    // Check pointers
    if ( pbmpBitmap == NULL )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    // Check parameters consistency
    if ( (dwWidth == 0) || (dwHeight == 0) || (dwBPP == 0) )
    {
        return SAMGUI_E_BAD_PARAMETER ;
    }

    memset( pbmpBitmap, 0, sizeof( SGUIBitmapRaw ) ) ;
    pbmpBitmap->dwWidth=dwWidth ;
    pbmpBitmap->dwHeight=dwHeight ;
    pbmpBitmap->dwBPP=dwBPP ;

    return SAMGUI_E_OK ;
}

extern int BitmapRaw_SetData( SGUIBitmapRaw* pbmpBitmap, uint8_t* pucData, uint32_t dwDataLength )
{
    // Check pointers
    if ( (pbmpBitmap == NULL) || (pucData == NULL) )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    // Check parameters consistency
    if ( dwDataLength == 0 )
    {
        return SAMGUI_E_BAD_PARAMETER ;
    }

    pbmpBitmap->pucData=pucData ;
    pbmpBitmap->dwDataLength=dwDataLength ;

    return SAMGUI_E_OK ;
}

extern int BitmapRaw_SetLUT( SGUIBitmapRaw* pbmpBitmap, uint8_t* pucLUT, uint32_t dwLUTLength )
{
    // Check pointers
    if ( (pbmpBitmap == NULL) || (pucLUT == NULL) )
    {
        return SAMGUI_E_BAD_POINTER ;
    }

    // Check parameters consistency
    if ( dwLUTLength == 0 )
    {
        return SAMGUI_E_BAD_PARAMETER ;
    }

    pbmpBitmap->pucData=pucLUT ;
    pbmpBitmap->dwDataLength=dwLUTLength ;

    return SAMGUI_E_OK ;
}
