#include <graphx.h>

#include <colors.h>

#include <ui/base.h>

inline void ui_DrawServerListBackground() {
    gfx_SetColor(COLOR_ServerList);
    gfx_FillRectangle_NoClip(0, 0, 22, GFX_LCD_HEIGHT);
}

inline void ui_DrawSidebar() {
    gfx_SetColor(COLOR_Sidebar);
    gfx_FillRectangle_NoClip(22, 0, 28, GFX_LCD_HEIGHT);
}