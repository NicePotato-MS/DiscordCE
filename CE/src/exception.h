#pragma once

#include <stdint.h>

#define CONDITION_SUCCESS 0
#define CONDITION_FONT_FAILURE 1
#define CONDITION_UNHANDLED_SERIAL_FAILURE 2
#define CONDITION_CORRUPTED_PACKET 3

#define EXCEPTION_TYPE_NONE 0
#define EXCEPTION_TYPE_STRING 1
#define EXCEPTION_TYPE_UINT 2
#define EXCEPTION_TYPE_INT 3
#define EXCEPTION_TYPE_NAMED_UINT 4
#define EXCEPTION_TYPE_NAMED_INT 5

#define EXCEPTION_INFO_ENTRIES 10

void exception_LogInfoString(uint8_t index, const char *str);
void exception_LogInfoUint(uint8_t index, unsigned int value);
void exception_LogInfoInt(uint8_t index, int value);
void exception_LogInfoNamedUint(uint8_t index, const char *str, unsigned int value);
void exception_LogInfoNamedInt(uint8_t index, const char *str, int value);

void exception_Cleanup();
void exception_Exit(int condition);
void exception_Crash(int condition);
extern void exception_Setup();