#include <stdio.h>

#include <keypadc.h>

#include "clock.h"
#include "keyboard.h"

// const char *keynames[] = {
//     // Offset 1 - 0-7
//     "graph",
//     "trace",
//     "zoom",
//     "window",
//     "y=",
//     "2nd",
//     "mode",
//     "del",
//     // Offset 2 - 8-15
//     "???",
//     "sto->",
//     "ln",
//     "log",
//     "x^2",
//     "x^-1",
//     "math",
//     "alpha",
//     // Offset 3 - 16-23
//     "0",
//     "1",
//     "4",
//     "7",
//     ",",
//     "sin",
//     "apps",
//     "X,T,0,n",
//     // Offset 4 - 24-31
//     ".",
//     "2",
//     "5",
//     "8",
//     "(",
//     "cos",
//     "prgm",
//     "stat",
//     // Offset 5 - 32-39
//     "(-)",
//     "3",
//     "6",
//     "9",
//     ")",
//     "tan",
//     "vars",
//     "???",
//     // Offset 6 - 40-47
//     "enter",
//     "+",
//     "-",
//     "x",
//     "/ (div)",
//     "^",
//     "clear",
//     "???",
//     // Offset 7 - 48-55
//     "Down",
//     "Left",
//     "Right",
//     "Up",
//     "???",
//     "???",
//     "???",
//     "???"
// };

clock_t keyboard_key_states[56]; 

unsigned int key_offset;

inline bool keyboard_HasKeyBeenHeldFor(unsigned int key, clock_t time) {
    return keyboard_key_states[key] && clock_ElapsedTimeSince(keyboard_key_states[key]) > time;
}

inline bool keyboard_IsKeyDown(unsigned int key) {
    return keyboard_key_states[key];
}

void keyboard_UpdateKeyStates() {
    kb_Scan();
    key_offset = 0;
    for (unsigned int keyGroup = 1; keyGroup<8; keyGroup++) {
        for (unsigned int currentKey = 0; currentKey<8; currentKey++) {
            if ((kb_Data[keyGroup] & (1 << currentKey)) != 0) {
                if(!keyboard_key_states[key_offset]) {
                    keyboard_key_states[key_offset] = clock_current_clock;
                }
            } else {keyboard_key_states[key_offset] = 0;}
            key_offset++;
        }
    }   
}