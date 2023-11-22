#pragma once

#include <sys/types.h>

// Keyboard keys
typedef enum
{
    BZF_KEY_UNKNOWN = 0,
    BZF_KEY_SPACE,
    BZF_KEY_APOSTROPHE,
    BZF_KEY_COMMA,
    BZF_KEY_MINUS,
    BZF_KEY_PERIOD,
    BZF_KEY_SLASH,
    BZF_KEY_0,
    BZF_KEY_1,
    BZF_KEY_2,
    BZF_KEY_3,
    BZF_KEY_4,
    BZF_KEY_5,
    BZF_KEY_6,
    BZF_KEY_7,
    BZF_KEY_8,
    BZF_KEY_9,
    BZF_KEY_SEMICOLON,
    BZF_KEY_EQUAL,
    BZF_KEY_A,
    BZF_KEY_B,
    BZF_KEY_C,
    BZF_KEY_D,
    BZF_KEY_E,
    BZF_KEY_F,
    BZF_KEY_G,
    BZF_KEY_H,
    BZF_KEY_I,
    BZF_KEY_J,
    BZF_KEY_K,
    BZF_KEY_L,
    BZF_KEY_M,
    BZF_KEY_N,
    BZF_KEY_O,
    BZF_KEY_P,
    BZF_KEY_Q,
    BZF_KEY_R,
    BZF_KEY_S,
    BZF_KEY_T,
    BZF_KEY_U,
    BZF_KEY_V,
    BZF_KEY_W,
    BZF_KEY_X,
    BZF_KEY_Y,
    BZF_KEY_Z,
    BZF_KEY_LEFT_BRACKET,
    BZF_KEY_BACKSLASH,
    BZF_KEY_RIGHT_BRACKET,
    BZF_KEY_GRAVE_ACCENT,
    BZF_KEY_WORLD_1,
    BZF_KEY_WORLD_2,
    BZF_KEY_ESCAPE,
    BZF_KEY_ENTER,
    BZF_KEY_TAB,
    BZF_KEY_BACKSPACE,
    BZF_KEY_INSERT,
    BZF_KEY_DELETE,
    BZF_KEY_RIGHT,
    BZF_KEY_LEFT,
    BZF_KEY_DOWN,
    BZF_KEY_UP,
    BZF_KEY_PAGE_UP,
    BZF_KEY_PAGE_DOWN,
    BZF_KEY_HOME,
    BZF_KEY_END,
    BZF_KEY_PAUSE,
    BZF_KEY_F1,
    BZF_KEY_F2,
    BZF_KEY_F3,
    BZF_KEY_F4,
    BZF_KEY_F5,
    BZF_KEY_F6,
    BZF_KEY_F7,
    BZF_KEY_F8,
    BZF_KEY_F9,
    BZF_KEY_F10,
    BZF_KEY_F11,
    BZF_KEY_F12,
    BZF_KEY_F13,
    BZF_KEY_F14,
    BZF_KEY_F15,
    BZF_KEY_F16,
    BZF_KEY_F17,
    BZF_KEY_F18,
    BZF_KEY_F19,
    BZF_KEY_F20,
    BZF_KEY_F21,
    BZF_KEY_F22,
    BZF_KEY_F23,
    BZF_KEY_F24,
    BZF_KEY_F25,
    BZF_KEY_KP_0,
    BZF_KEY_KP_1,
    BZF_KEY_KP_2,
    BZF_KEY_KP_3,
    BZF_KEY_KP_4,
    BZF_KEY_KP_5,
    BZF_KEY_KP_6,
    BZF_KEY_KP_7,
    BZF_KEY_KP_8,
    BZF_KEY_KP_9,
    BZF_KEY_KP_DECIMAL,
    BZF_KEY_KP_DIVIDE,
    BZF_KEY_KP_MULTIPLY,
    BZF_KEY_KP_SUBTRACT,
    BZF_KEY_KP_ADD,
    BZF_KEY_KP_ENTER,
    BZF_KEY_KP_EQUAL,
    BZF_KEY_LEFT_SHIFT,
    BZF_KEY_LEFT_CONTROL,
    BZF_KEY_LEFT_ALT,
    BZF_KEY_LEFT_SUPER,
    BZF_KEY_RIGHT_SHIFT,
    BZF_KEY_RIGHT_CONTROL,
    BZF_KEY_RIGHT_ALT,
    BZF_KEY_RIGHT_SUPER,
    BZF_KEY_MENU,
    BZF_KEY_LAST = BZF_KEY_MENU
} BzfKey;

// Mouse buttons
typedef enum
{
    BZF_MOUSE_UNKNOWN = 0,
    BZF_MOUSE_LEFT,
    BZF_MOUSE_MIDDLE,
    BZF_MOUSE_RIGHT,
    BZF_MOUSE_4,
    BZF_MOUSE_5,
    BZF_MOUSE_6,
    BZF_MOUSE_7,
    BZF_MOUSE_8,
    BZF_MOUSE_9,
    BZF_MOUSE_10,
    BZF_MOUSE_LAST = BZF_MOUSE_10
} BzfMouseButton;

// Joystick buttons
typedef enum
{
    BZF_JOY_BUTTON_UNKNOWN = 0,
    BZF_JOY_BUTTON_1,
    BZF_JOY_BUTTON_2,
    BZF_JOY_BUTTON_3,
    BZF_JOY_BUTTON_4,
    BZF_JOY_BUTTON_5,
    BZF_JOY_BUTTON_6,
    BZF_JOY_BUTTON_7,
    BZF_JOY_BUTTON_8,
    BZF_JOY_BUTTON_9,
    BZF_JOY_BUTTON_10,
    BZF_JOY_BUTTON_11,
    BZF_JOY_BUTTON_12,
    BZF_JOY_BUTTON_13,
    BZF_JOY_BUTTON_14,
    BZF_JOY_BUTTON_15,
    BZF_JOY_BUTTON_16,
    BZF_JOY_BUTTON_17,
    BZF_JOY_BUTTON_18,
    BZF_JOY_BUTTON_19,
    BZF_JOY_BUTTON_20,
    BZF_JOY_BUTTON_21,
    BZF_JOY_BUTTON_22,
    BZF_JOY_BUTTON_23,
    BZF_JOY_BUTTON_24,
    BZF_JOY_BUTTON_25,
    BZF_JOY_BUTTON_26,
    BZF_JOY_BUTTON_27,
    BZF_JOY_BUTTON_28,
    BZF_JOY_BUTTON_29,
    BZF_JOY_BUTTON_30,
    BZF_JOY_BUTTON_31,
    BZF_JOY_BUTTON_32,
    BZF_JOY_LAST_BUTTON = BZF_JOY_BUTTON_32
} BzfJoyButton;

typedef enum
{
    BZF_JOY_HAT_UNKNOWN = 0,
    BZF_JOY_HAT_1,
    BZF_JOY_HAT_2,
    BZF_JOY_HAT_3,
    BZF_JOY_HAT_4,
    BZF_JOY_HAT_5,
    BZF_JOY_HAT_6,
    BZF_JOY_HAT_7,
    BZF_JOY_HAT_8,
    BZF_JOY_LAST_HAT = BZF_JOY_HAT_8
} BzfJoyHat;

typedef enum
{
    BZF_JOY_HAT_CENTERED = 0x00,
    BZF_JOY_HAT_UP = 0x01,
    BZF_JOY_HAT_RIGHT = 0x02,
    BZF_JOY_HAT_DOWN = 0x04,
    BZF_JOY_HAT_LEFT = 0x08,
    BZF_JOY_HAT_RIGHTUP = (BZF_JOY_HAT_RIGHT|BZF_JOY_HAT_UP),
    BZF_JOY_HAT_RIGHTDOWN = (BZF_JOY_HAT_RIGHT|BZF_JOY_HAT_DOWN),
    BZF_JOY_HAT_LEFTUP = (BZF_JOY_HAT_LEFT|BZF_JOY_HAT_UP),
    BZF_JOY_HAT_LEFTDOWN = (BZF_JOY_HAT_LEFT|BZF_JOY_HAT_DOWN)
} BzfJoyHatDirection;


// Key modifiers
enum
{
    BZF_MOD_CTRL = 1<<1,
    BZF_MOD_SHIFT = 1<<2,
    BZF_MOD_ALT = 1<<3,
    BZF_MOD_SUPER = 1<<4
};

// Key actions
typedef enum
{
    BZF_KEY_RELEASED = 0,
    BZF_KEY_PRESSED,
    BZF_KEY_REPEATED
} BzfKeyAction;

// Button actions
typedef enum
{
    BZF_BUTTON_RELEASED = 0,
    BZF_BUTTON_PRESSED
} BzfButtonAction;

inline const char* getKeyName(BzfKey key)
{
    switch (key)
    {
    // *INDENT-OFF*
    case BZF_KEY_SPACE: return "Space";
    case BZF_KEY_APOSTROPHE: return "'";
    case BZF_KEY_COMMA: return ",";
    case BZF_KEY_MINUS: return "-";
    case BZF_KEY_PERIOD: return ".";
    case BZF_KEY_SLASH: return "/";
    case BZF_KEY_0: return "0";
    case BZF_KEY_1: return "1";
    case BZF_KEY_2: return "2";
    case BZF_KEY_3: return "3";
    case BZF_KEY_4: return "4";
    case BZF_KEY_5: return "5";
    case BZF_KEY_6: return "6";
    case BZF_KEY_7: return "7";
    case BZF_KEY_8: return "8";
    case BZF_KEY_9: return "9";
    case BZF_KEY_SEMICOLON: return ";";
    case BZF_KEY_EQUAL: return "=";
    case BZF_KEY_A: return "A";
    case BZF_KEY_B: return "B";
    case BZF_KEY_C: return "C";
    case BZF_KEY_D: return "D";
    case BZF_KEY_E: return "E";
    case BZF_KEY_F: return "F";
    case BZF_KEY_G: return "G";
    case BZF_KEY_H: return "H";
    case BZF_KEY_I: return "I";
    case BZF_KEY_J: return "J";
    case BZF_KEY_K: return "K";
    case BZF_KEY_L: return "L";
    case BZF_KEY_M: return "M";
    case BZF_KEY_N: return "N";
    case BZF_KEY_O: return "O";
    case BZF_KEY_P: return "P";
    case BZF_KEY_Q: return "Q";
    case BZF_KEY_R: return "R";
    case BZF_KEY_S: return "S";
    case BZF_KEY_T: return "T";
    case BZF_KEY_U: return "U";
    case BZF_KEY_V: return "V";
    case BZF_KEY_W: return "W";
    case BZF_KEY_X: return "X";
    case BZF_KEY_Y: return "Y";
    case BZF_KEY_Z: return "Z";
    case BZF_KEY_LEFT_BRACKET: return "[";
    case BZF_KEY_BACKSLASH: return "\\";
    case BZF_KEY_RIGHT_BRACKET: return "]";
    case BZF_KEY_GRAVE_ACCENT: return "`";
    case BZF_KEY_WORLD_1: return "????"; // What would these be??
    case BZF_KEY_WORLD_2: return "????"; // What would these be??
    case BZF_KEY_ESCAPE: return "Escape";
    case BZF_KEY_ENTER: return "Enter";
    case BZF_KEY_TAB: return "Tab";
    case BZF_KEY_BACKSPACE: return "Backspace";
    case BZF_KEY_INSERT: return "Insert";
    case BZF_KEY_DELETE: return "Delete";
    case BZF_KEY_RIGHT: return "Right";
    case BZF_KEY_LEFT: return "Left";
    case BZF_KEY_DOWN: return "Down";
    case BZF_KEY_UP: return "Up";
    case BZF_KEY_PAGE_UP: return "Page UP";
    case BZF_KEY_PAGE_DOWN: return "Page Down";
    case BZF_KEY_HOME: return "Home";
    case BZF_KEY_END: return "End";
    case BZF_KEY_PAUSE: return "Pause";
    case BZF_KEY_F1: return "F1";
    case BZF_KEY_F2: return "F2";
    case BZF_KEY_F3: return "F3";
    case BZF_KEY_F4: return "F4";
    case BZF_KEY_F5: return "F5";
    case BZF_KEY_F6: return "F6";
    case BZF_KEY_F7: return "F7";
    case BZF_KEY_F8: return "F8";
    case BZF_KEY_F9: return "F9";
    case BZF_KEY_F10: return "F10";
    case BZF_KEY_F11: return "F11";
    case BZF_KEY_F12: return "F12";
    case BZF_KEY_F13: return "F13";
    case BZF_KEY_F14: return "F14";
    case BZF_KEY_F15: return "F15";
    case BZF_KEY_F16: return "F16";
    case BZF_KEY_F17: return "F17";
    case BZF_KEY_F18: return "F18";
    case BZF_KEY_F19: return "F19";
    case BZF_KEY_F20: return "F20";
    case BZF_KEY_F21: return "F21";
    case BZF_KEY_F22: return "F22";
    case BZF_KEY_F23: return "F23";
    case BZF_KEY_F24: return "F24";
    case BZF_KEY_F25: return "F25";
    case BZF_KEY_KP_0: return "Keypad 0";
    case BZF_KEY_KP_1: return "Keypad 1";
    case BZF_KEY_KP_2: return "Keypad 2";
    case BZF_KEY_KP_3: return "Keypad 3";
    case BZF_KEY_KP_4: return "Keypad 4";
    case BZF_KEY_KP_5: return "Keypad 5";
    case BZF_KEY_KP_6: return "Keypad 6";
    case BZF_KEY_KP_7: return "Keypad 7";
    case BZF_KEY_KP_8: return "Keypad 8";
    case BZF_KEY_KP_9: return "Keypad 9";
    case BZF_KEY_KP_DECIMAL: return "Keypad Decimal";
    case BZF_KEY_KP_DIVIDE: return "Keypad Divide";
    case BZF_KEY_KP_MULTIPLY: return "Keypad Multiply";
    case BZF_KEY_KP_SUBTRACT: return "Keypad Subtract";
    case BZF_KEY_KP_ADD: return "Keypad Add";
    case BZF_KEY_KP_ENTER: return "Keypad Enter";
    case BZF_KEY_KP_EQUAL: return "Keypad Equal";
    case BZF_KEY_LEFT_SHIFT: return "Left Shift";
    case BZF_KEY_LEFT_CONTROL: return "Left Control";
    case BZF_KEY_LEFT_ALT: return "Left Alt";
    case BZF_KEY_LEFT_SUPER: return "Left Super";
    case BZF_KEY_RIGHT_SHIFT: return "Right Shift";
    case BZF_KEY_RIGHT_CONTROL: return "Right Control";
    case BZF_KEY_RIGHT_ALT: return "Right Alt";
    case BZF_KEY_RIGHT_SUPER: return "Right Super";
    case BZF_KEY_MENU: return "Menu";
    default: return nullptr;
    // *INDENT-ON*
    }
}

inline const char* getMouseButtonName(BzfMouseButton button)
{
    switch(button)
    {
    // *INDENT-OFF*
    case BZF_MOUSE_LEFT: return "Left Mouse";
    case BZF_MOUSE_MIDDLE: return "Middle Mouse";
    case BZF_MOUSE_RIGHT: return "Right Mouse";
    case BZF_MOUSE_4: return "Mouse 4";
    case BZF_MOUSE_5: return "Mouse 5";
    case BZF_MOUSE_6: return "Mouse 6";
    case BZF_MOUSE_7: return "Mouse 7";
    case BZF_MOUSE_8: return "Mouse 8";
    case BZF_MOUSE_9: return "Mouse 0";
    case BZF_MOUSE_10: return "Mouse 10";
    default: return nullptr;
    // *INDENT-ON*
    }
}

inline const char* getJoystickButtonName(BzfJoyButton button)
{
    switch (button)
    {
    // *INDENT-OFF*
    case BZF_JOY_BUTTON_1: return "Button 1";
    case BZF_JOY_BUTTON_2: return "Button 2";
    case BZF_JOY_BUTTON_3: return "Button 3";
    case BZF_JOY_BUTTON_4: return "Button 4";
    case BZF_JOY_BUTTON_5: return "Button 5";
    case BZF_JOY_BUTTON_6: return "Button 6";
    case BZF_JOY_BUTTON_7: return "Button 7";
    case BZF_JOY_BUTTON_8: return "Button 8";
    case BZF_JOY_BUTTON_9: return "Button 9";
    case BZF_JOY_BUTTON_10: return "Button 10";
    case BZF_JOY_BUTTON_11: return "Button 11";
    case BZF_JOY_BUTTON_12: return "Button 12";
    case BZF_JOY_BUTTON_13: return "Button 13";
    case BZF_JOY_BUTTON_14: return "Button 14";
    case BZF_JOY_BUTTON_15: return "Button 15";
    case BZF_JOY_BUTTON_16: return "Button 16";
    case BZF_JOY_BUTTON_17: return "Button 17";
    case BZF_JOY_BUTTON_18: return "Button 18";
    case BZF_JOY_BUTTON_19: return "Button 19";
    case BZF_JOY_BUTTON_20: return "Button 20";
    case BZF_JOY_BUTTON_21: return "Button 21";
    case BZF_JOY_BUTTON_22: return "Button 22";
    case BZF_JOY_BUTTON_23: return "Button 23";
    case BZF_JOY_BUTTON_24: return "Button 24";
    case BZF_JOY_BUTTON_25: return "Button 25";
    case BZF_JOY_BUTTON_26: return "Button 26";
    case BZF_JOY_BUTTON_27: return "Button 27";
    case BZF_JOY_BUTTON_28: return "Button 28";
    case BZF_JOY_BUTTON_29: return "Button 29";
    case BZF_JOY_BUTTON_30: return "Button 30";
    case BZF_JOY_BUTTON_31: return "Button 31";
    case BZF_JOY_BUTTON_32: return "Button 32";
    default: return nullptr;
    // *INDENT-ON*
    }
}

inline const char * getJoystickHatName(BzfJoyHat hat)
{
    switch (hat)
    {
    // *INDENT-OFF*
    case BZF_JOY_HAT_1: return "Hat 1";
    case BZF_JOY_HAT_2: return "Hat 2";
    case BZF_JOY_HAT_3: return "Hat 3";
    case BZF_JOY_HAT_4: return "Hat 4";
    case BZF_JOY_HAT_5: return "Hat 5";
    case BZF_JOY_HAT_6: return "Hat 6";
    case BZF_JOY_HAT_7: return "Hat 7";
    case BZF_JOY_HAT_8: return "Hat 8";
    default: return nullptr;
    // *INDENT-ON*
    }
}

inline const char * getJoystickHatDirectionName(BzfJoyHatDirection direction)
{
    switch(direction)
    {
    // *INDENT-OFF*
    case BZF_JOY_HAT_LEFTUP: return "left up";
    case BZF_JOY_HAT_UP: return "up";
    case BZF_JOY_HAT_RIGHTUP: return "right up";
    case BZF_JOY_HAT_LEFT: return "left";
    case BZF_JOY_HAT_RIGHT: return "right";
    case BZF_JOY_HAT_LEFTDOWN: return "left down";
    case BZF_JOY_HAT_DOWN: return "down";
    case BZF_JOY_HAT_RIGHTDOWN: return "right down";
    default: return "centered";
    // *INDENT-ON*
    }
}
