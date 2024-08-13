#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <graphx.h>

#include "exception.h"
#include "keyboard.h"
#include "serial.h"

const char *exception_condition_strings[] = {
    "Manually triggered crash",
    "Failed to load DrMono font! Install it",
    "Unhandled Serial Error",
    "Received Corrupted Packet",
};

typedef union {
    void *ptr;
    unsigned int unsigned_num;
    int signed_num;
} exception_InfoData;

typedef struct {
    uint8_t type;
    exception_InfoData data_1;
    exception_InfoData data_2;
} exception_Info;

exception_Info exception_InfoArray[EXCEPTION_INFO_ENTRIES];

void exception_LogInfoString(uint8_t index, const char *str) {
    exception_Info *info = &exception_InfoArray[index];
    info->type = EXCEPTION_TYPE_STRING;
    info->data_1.ptr = (void *)str;
    info->data_2.ptr = NULL;
}

void exception_LogInfoUint(uint8_t index, unsigned int value) {
    exception_Info *info = &exception_InfoArray[index];
    info->type = EXCEPTION_TYPE_UINT;
    info->data_1.unsigned_num = value;
    info->data_2.ptr = NULL;
}

void exception_LogInfoInt(uint8_t index, int value) {
    exception_Info *info = &exception_InfoArray[index];
    info->type = EXCEPTION_TYPE_INT;
    info->data_1.signed_num = value;
    info->data_2.ptr = NULL;
}

void exception_LogInfoNamedUint(uint8_t index, const char *str, unsigned int value) {
    exception_Info *info = &exception_InfoArray[index];
    info->type = EXCEPTION_TYPE_NAMED_UINT;
    info->data_1.ptr = (void *)str;
    info->data_2.unsigned_num = value;
}

void exception_LogInfoNamedInt(uint8_t index, const char *str, int value) {
    exception_Info *info = &exception_InfoArray[index];
    info->type = EXCEPTION_TYPE_NAMED_INT;
    info->data_1.ptr = (void *)str;
    info->data_2.signed_num = value;
}


void exception_Cleanup() {
    usb_Cleanup();
    gfx_End();
}

void exception_Exit(int condition) {
    exception_Cleanup();
    exit(condition);
}

void exception_Crash(int condition) {
    gfx_SetDefaultPalette(gfx_8bpp);
    gfx_FillScreen(0); // black

    gfx_SetTextBGColor(0); // black
    gfx_SetTextFGColor(0xE3); // orange
    gfx_SetTextConfig(gfx_text_noclip);
    gfx_SetClipRegion(0, 0, GFX_LCD_WIDTH, GFX_LCD_HEIGHT);
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("**EXCEPTION**", 56, 2);
    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY("Press clear to close", GFX_LCD_WIDTH - 141, GFX_LCD_HEIGHT - 11);
    gfx_SetTextConfig(gfx_text_clip);
    gfx_PrintStringXY(exception_condition_strings[condition], 2, 20);
    serial_SendPacketHeader(0, PACKET_EXCEPTION_OCCURED);
    serial_DebugPrint(exception_condition_strings[condition]);

    for (unsigned int i = 0; i < EXCEPTION_INFO_ENTRIES; i++) {
        exception_Info *info = &exception_InfoArray[i];
        uint8_t y = 30 + (i * 10);
        unsigned int x;
        switch (info->type) {
            case EXCEPTION_TYPE_STRING:
                gfx_PrintStringXY(info->data_1.ptr, 2, y);
                serial_DebugPrint(info->data_1.ptr);
                break;
            case EXCEPTION_TYPE_UINT:
                gfx_SetTextXY(2, y);
                gfx_PrintUInt(info->data_1.unsigned_num, 1);
                serial_DebugPrintUnsigned(info->data_1.unsigned_num);
                break;
            case EXCEPTION_TYPE_INT:
                gfx_SetTextXY(2, y);
                gfx_PrintInt(info->data_1.signed_num, 1);
                serial_DebugPrintSigned(info->data_1.signed_num);
                break;
            case EXCEPTION_TYPE_NAMED_UINT:
                gfx_PrintStringXY(info->data_1.ptr, 2, y);
                x = 2 + (strlen(info->data_1.ptr) * 7) + 7;
                gfx_SetTextXY(x, y);
                gfx_PrintChar(':');
                gfx_SetTextXY(x + 14, y);
                gfx_PrintUInt(info->data_2.unsigned_num, 1);
                serial_DebugPrintNamedUnsigned(info->data_1.ptr, info->data_2.unsigned_num);
                break;
            case EXCEPTION_TYPE_NAMED_INT:
                gfx_PrintStringXY(info->data_1.ptr, 2, y);
                x = 2 + (strlen(info->data_1.ptr) * 7) + 7;
                gfx_SetTextXY(x, y);
                gfx_PrintChar(':');
                gfx_SetTextXY(x + 14, y);
                gfx_PrintInt(info->data_2.signed_num, 1);
                serial_DebugPrintNamedSigned(info->data_1.ptr, info->data_2.signed_num);
                break;
        }
    }

    do {
        keyboard_UpdateKeyStates();
        usb_HandleEvents();
    } while (!keyboard_key_states[KEY_CLEAR]);

    exception_Exit(condition);
}

inline void exception_Setup() {
    for (unsigned int i = 0; i < EXCEPTION_INFO_ENTRIES; i++) {
        exception_InfoArray[i].type = EXCEPTION_TYPE_NONE;
    }
}