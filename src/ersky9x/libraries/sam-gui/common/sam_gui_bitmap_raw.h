#ifndef _SAM_GUI_BITMAP_RAW_
#define _SAM_GUI_BITMAP_RAW_

typedef struct _SGUIBitmapRaw
{
    uint32_t dwWidth ;
    uint32_t dwHeight ;
    uint32_t dwBPP ;
    uint32_t dwLUTLength ;
    uint8_t* pucLUT ;
    uint32_t dwDataLength ;
    uint8_t* pucData ;
} SGUIBitmapRaw ;

extern int BitmapRaw_Initialize( SGUIBitmapRaw* pbmpBitmap, uint32_t dwWidth, uint32_t dwHeight,  uint32_t dwBPP ) ;

extern int BitmapRaw_SetData( SGUIBitmapRaw* pbmpBitmap, uint8_t* pucData, uint32_t dwDataLength ) ;
extern int BitmapRaw_SetLUT( SGUIBitmapRaw* pbmpBitmap, uint8_t* pucLUT, uint32_t dwLUTLength ) ;

#endif // _SAM_GUI_BITMAP_RAW_
