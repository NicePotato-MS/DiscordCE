#pragma once

#include <keypadc.h>
#include <time.h>

#define KEY_GRAPH 0
#define KEY_TRACE 1
#define KEY_ZOOM 2
#define KEY_WINDOW 3
#define KEY_YEQU 4
#define KEY_2ND 5
#define KEY_MODE 6
#define KEY_DEL 7

#define KEU_STO 9
#define KEY_LN 10
#define KEY_LOG 11
#define KEY_X_POWER_2 12
#define KEY_X_POWER_MINUS_1 13
#define KEY_MATH 14
#define KEY_ALPHA 15

#define KEY_0 16
#define KEY_1 17
#define KEY_4 18
#define KEY_7 19
#define KEY_COMMA 20
#define KEY_SIN 21
#define KEY_APPS 22
#define KEY_XT0n 23

#define KEY_PERIOD 24
#define KEY_2 25
#define KEY_5 26
#define KEY_8 27
#define KEY_LEFT_PAREN 28
#define KEY_COS 29
#define KEY_PRGM 30
#define KEY_STAT 31

#define KEY_NEGATIVE 32
#define KEY_3  33
#define KEY_6 34
#define KEY_9 35
#define KEY_RIGHT_PAREN 36
#define KEY_TAN 37
#define KEY_VARS 38

#define KEY_ENTER 40
#define KEY_PLUS 41
#define KEY_MINUS 42
#define KEY_TIMES 43
#define KEY_DIVIDE 44
#define KEY_CARET 45
#define KEY_CLEAR 46

#define KEY_DOWN 48
#define KEY_LEFT 49
#define KEY_RIGHT 50
#define KEY_UP 51

// extern const char *keynames[];

extern clock_t keyboard_key_states[56];

extern bool keyboard_HasKeyBeenHeldFor(unsigned int key, clock_t time);
extern bool keyboard_IsKeyDown(unsigned int key);

void keyboard_UpdateKeyStates();