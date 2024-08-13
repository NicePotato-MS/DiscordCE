#include <graphx.h>
#include <keypadc.h>
#include <usbdrvce.h>

#include <gfx/gfx.h>

#include "exception.h"
#include "colors.h"
#include "keyboard.h"
#include "clock.h"
#include "font.h"
#include "serial.h"

#include "ui/base.h"

void loop() {
    clock_SetClock();
    keyboard_UpdateKeyStates();

    fontlib_SetFont(font_drmono_regular, 0);
    fontlib_SetColors(COLOR_White, COLOR_Background);
    fontlib_SetCursorPosition(55, 5);
    if (serial_has_device) {
        fontlib_DrawString("Serial CONNECTED   ");
    } else {
        fontlib_DrawString("Serial DISCONNECTED");
    }

    if (keyboard_key_states[KEY_CLEAR]) { exception_Exit(CONDITION_SUCCESS); }

    usb_HandleEvents();
    if (serial_has_device) {
        if (keyboard_key_states[KEY_YEQU]) {
            while (keyboard_key_states[KEY_YEQU]) { keyboard_UpdateKeyStates(); }
            serial_SendPacketHeader(0, PACKET_DEBUG_REQUEST_PFP_TEST);
        }

        serial_ReceivePacket();
    }

    if (keyboard_HasKeyBeenHeldFor(KEY_MODE, CLOCKS_PER_SEC)) {
        exception_LogInfoInt(0, -420);
        exception_Crash(CONDITION_SUCCESS);
    }
}

int main(void) {
    gfx_Begin();
    gfx_SetPalette(main_palette, sizeof_main_palette, 0);
    gfx_ZeroScreen();

    font_LoadFonts();

    ui_DrawServerListBackground();
    ui_DrawSidebar();
    gfx_Sprite_NoClip(pic_discord_selected, 3, 3);

    serial_Setup();
    
    while (1) { loop(); }
}