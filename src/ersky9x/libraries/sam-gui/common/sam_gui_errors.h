#ifndef _SAM_GUI_ERRORS_
#define _SAM_GUI_ERRORS_

#define SAMGUI_E_OK                              0x0L

#define SAMGUI_ERRORS_COMMON_BASE                0x1L
#define SAMGUI_ERRORS_PORTING_BASE               0x1000L
#define SAMGUI_ERRORS_DISP_BASE                  0x2000L
#define SAMGUI_ERRORS_WGT_BASE                   0x3000L
#define SAMGUI_ERRORS_WM_BASE                    0x4000L


/*
 * Common errors
 */
#define SAMGUI_E_BAD_POINTER                     SAMGUI_ERRORS_COMMON_BASE
#define SAMGUI_E_BAD_PARAMETER                   SAMGUI_ERRORS_COMMON_BASE+1
#define SAMGUI_E_FILE_OPEN                       SAMGUI_ERRORS_COMMON_BASE+2

/*
 * Porting layer errors
 */

/*
 * DISP layer errors
 */
#define SAMGUI_E_WRONG_COMPONENT                 SAMGUI_ERRORS_DISP_BASE

/*
 * WGT layer errors
 */
#define SAMGUI_E_TOO_MUCH_WIDGETS                SAMGUI_ERRORS_WGT_BASE
#define SAMGUI_E_MSQUEUE                         SAMGUI_ERRORS_WGT_BASE+1
#define SAMGUI_E_NO_SELECTED_WIDGET              SAMGUI_ERRORS_WGT_BASE+2
#define SAMGUI_E_NOT_ENOUGH_MEMORY               SAMGUI_ERRORS_WGT_BASE+3
#define SAMGUI_E_INVALID_WIDGET                  SAMGUI_ERRORS_WGT_BASE+4

/*
 * WM layer errors
 */

#endif // _SAM_GUI_ERRORS_
