#ifndef _SAM_GUI_COLOR_
#define _SAM_GUI_COLOR_

typedef struct _SGUIColor
{
    union _u
    {
        uint32_t dwRGBA ;

        struct _RGBA
        {
            uint8_t ucA ;
            uint8_t ucB ;
            uint8_t ucG ;
            uint8_t ucR ;
        } RGBA ;
    } u ;
} SGUIColor ;

//#define ucR u.RGBA.ucR
//#define ucG u.RGBA.ucG
//#define ucB u.RGBA.ucB
//#define ucA u.RGBA.ucA

extern int COLOR_GetGradientValue( SGUIColor* pclrBegin, SGUIColor* pclrEnd,
                                   uint32_t dwNumber, uint32_t dwIndex, SGUIColor* pclrResult ) ;

extern int COLOR_RGB565( SGUIColor* pclrIn, SGUIColor* pclrResult ) ;

//#define COLOR_FROM_RGB( r, g, b )

#endif // _SAM_GUI_COLOR_
