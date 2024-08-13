#include <fontlibc.h>

#include "font.h"
#include "exception.h"

fontlib_font_t *font_drmono_regular;
fontlib_font_t *font_drmono_bold;

inline void font_LoadFonts() {
    font_drmono_regular = fontlib_GetFontByIndex("DrMono", 0);
    font_drmono_bold = fontlib_GetFontByIndex("DrMono", 1);
    if (!font_drmono_regular || !font_drmono_bold) { exception_Crash(CONDITION_FONT_FAILURE); }
}