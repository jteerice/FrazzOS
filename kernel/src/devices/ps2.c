#include "ps2.h"
#include "ioapic.h"
#include "status.h"
#include "klibc/string.h"
#include "klibc/io.h"
#include "cpu.h"

static bool ctrl = false;
static bool alt = false;
static bool shift = false;
static bool capslock = false;
static bool numlock = false;
static bool scroll_lock = false;

static uint32_t keycodes[] = {
//  key			scancode
    KEY_UNKNOWN,	//0x0
    KEY_ESCAPE,		//0x1
    KEY_1,		//0x2
    KEY_2,		//0x3
    KEY_3,		//0x4
    KEY_4,		//0x5
    KEY_5,		//0x6
    KEY_6,		//0x7
    KEY_7,		//0x8
    KEY_8,		//0x9
    KEY_9,		//0xa
    KEY_0,		//0xb
    KEY_MINUS,		//0xc
    KEY_EQUAL,		//0xd
    KEY_BACKSPACE,	//0xe
    KEY_TAB,		//0xf
    KEY_Q,		//0x10
    KEY_W,		//0x11
    KEY_E,		//0x12
    KEY_R,		//0x13
    KEY_T,		//0x14
    KEY_Y,		//0x15
    KEY_U,		//0x16
    KEY_I,		//0x17
    KEY_O,		//0x18
    KEY_P,		//0x19
    KEY_LEFTBRACKET,	//0x1a
    KEY_RIGHTBRACKET,	//0x1b
    KEY_RETURN,		//0x1c
    KEY_LCTRL,		//0x1d
    KEY_A,		//0x1e
    KEY_S,		//0x1f
    KEY_D,		//0x20
    KEY_F,		//0x21
    KEY_G,		//0x22
    KEY_H,		//0x23
    KEY_J,		//0x24
    KEY_K,		//0x25
    KEY_L,		//0x26
    KEY_SEMICOLON,	//0x27
    KEY_QUOTE,		//0x28
    KEY_GRAVE,		//0x29
    KEY_LSHIFT,		//0x2a
    KEY_BACKSLASH,	//0x2b
    KEY_Z,		//0x2c
    KEY_X,		//0x2d
    KEY_C,		//0x2e
    KEY_V,		//0x2f
    KEY_B,		//0x30
    KEY_N,		//0x31
    KEY_M,		//0x32
    KEY_COMMA,		//0x33
    KEY_DOT,		//0x34
    KEY_SLASH,		//0x35
    KEY_RSHIFT,		//0x36
    KEY_KP_ASTERISK,	//0x37
    KEY_RALT,		//0x38
    KEY_SPACE,		//0x39
    KEY_CAPSLOCK,	//0x3a
    KEY_F1,		//0x3b
    KEY_F2,		//0x3c
    KEY_F3,		//0x3d
    KEY_F4,		//0x3e
    KEY_F5,		//0x3f
    KEY_F6,		//0x40
    KEY_F7,		//0x41
    KEY_F8,		//0x42
    KEY_F9,		//0x43
    KEY_F10,		//0x44
    KEY_KP_NUMLOCK,	//0x45
    KEY_SCROLLLOCK,	//0x46
    KEY_HOME,		//0x47
    KEY_KP_8,		//0x48
    KEY_PAGEUP,		//0x49
    KEY_KP_2,		//0x50
    KEY_KP_3,		//0x51
    KEY_KP_0,		//0x52
    KEY_KP_DECIMAL,	//0x53
    KEY_UNKNOWN,	//0x54
    KEY_UNKNOWN,	//0x55
    KEY_UNKNOWN,	//0x56
    KEY_F11,		//0x57
    KEY_F12		//0x58
};

char keycode_to_ascii(uint32_t code) {
    uint8_t character = code;
    if (shift && capslock)
    {
        if (character == '0')
            character = KEY_RIGHTPARENTHESIS;
        else if (character == '1')
            character = KEY_EXCLAMATION;
        else if (character == '2')
            character = KEY_AT;
        else if (character == '3')
            character = KEY_HASH;
        else if (character == '4')
            character = KEY_DOLLAR;
        else if (character == '5')
            character = KEY_PERCENT;
        else if (character == '6')
            character = KEY_CARRET;
        else if (character == '7')
            character = KEY_AMPERSAND;
        else if (character == '8')
            character = KEY_ASTERISK;
        else if (character == '9')
            character = KEY_LEFTPARENTHESIS;
        else if (character == KEY_COMMA)
            character = KEY_LESS;
        else if (character == KEY_DOT)
            character = KEY_GREATER;
        else if (character == KEY_SLASH)
            character = KEY_QUESTION;
        else if (character == KEY_SEMICOLON)
            character = KEY_COLON;
        else if (character == KEY_QUOTE)
            character = KEY_QUOTEDOUBLE;
        else if (character == KEY_LEFTBRACKET)
            character = KEY_LEFTCURL;
        else if (character == KEY_RIGHTBRACKET)
            character = KEY_RIGHTCURL;
        else if (character == KEY_GRAVE)
            character = KEY_TILDE;
        else if (character == KEY_MINUS)
            character = KEY_UNDERSCORE;
        else if (character == KEY_EQUAL)
            character = KEY_PLUS;
        else if (character == KEY_BACKSLASH)
            character = KEY_BAR;
    }
    else if (shift && !capslock)
    {
        if (character >= 'a' && character <= 'z')
            character -= 32;

        if (character == '0')
            character = KEY_RIGHTPARENTHESIS;
        else if (character == '1')
            character = KEY_EXCLAMATION;
        else if (character == '2')
            character = KEY_AT;
        else if (character == '3')
            character = KEY_HASH;
        else if (character == '4')
            character = KEY_DOLLAR;
        else if (character == '5')
            character = KEY_PERCENT;
        else if (character == '6')
            character = KEY_CARRET;
        else if (character == '7')
            character = KEY_AMPERSAND;
        else if (character == '8')
            character = KEY_ASTERISK;
        else if (character == '9')
            character = KEY_LEFTPARENTHESIS;
        else if (character == KEY_COMMA)
            character = KEY_LESS;
        else if (character == KEY_DOT)
            character = KEY_GREATER;
        else if (character == KEY_SLASH)
            character = KEY_QUESTION;
        else if (character == KEY_SEMICOLON)
            character = KEY_COLON;
        else if (character == KEY_QUOTE)
            character = KEY_QUOTEDOUBLE;
        else if (character == KEY_LEFTBRACKET)
            character = KEY_LEFTCURL;
        else if (character == KEY_RIGHTBRACKET)
            character = KEY_RIGHTCURL;
        else if (character == KEY_GRAVE)
            character = KEY_TILDE;
        else if (character == KEY_MINUS)
            character = KEY_UNDERSCORE;
        else if (character == KEY_EQUAL)
            character = KEY_PLUS;
        else if (character == KEY_BACKSLASH)
            character = KEY_BAR;

    }
    else if (!shift && capslock)
    {
        if (character >= 'a' && character <= 'z')
            character -= 32;
    }

    return character;
}

void print_key(struct key key) {
    char key_buf[2];
    key_buf[0] = key.character;
    key_buf[1] = '\0';
    kprint(key_buf);
}

void keyboard_handler() {
    uint8_t scancode = inb(PS2_DATA_PORT);

    struct key key = {
        .keycode = KEY_UNKNOWN,
        .character = '\0'
    };

    if (scancode & BREAK_MODE_QUALIFIER) {
        scancode -= BREAK_MODE_QUALIFIER;
        uint32_t key_info = keycodes[scancode];
        if (key_info == KEY_LCTRL || key_info == KEY_RCTRL) {
            ctrl = false;
        } else if (key_info == KEY_LALT || key_info == KEY_RALT) {
            alt = false;
        } else if (key_info == KEY_LSHIFT || key_info == KEY_RSHIFT) {
            shift = false;
        }
    } else {
        uint32_t key_info = keycodes[scancode];
        key.keycode = key_info;
        if (key_info == KEY_LCTRL || key_info == KEY_RCTRL) {
            ctrl = true;
        } else if (key_info == KEY_LALT || key_info == KEY_RALT) {
            alt = true;
        } else if (key_info == KEY_LSHIFT || key_info == KEY_RSHIFT) {
            shift = true;
        } else if (key_info == KEY_CAPSLOCK) {
            capslock = capslock ? false : true;
        } else if (key_info == KEY_KP_NUMLOCK) {
            numlock = numlock ? false : true;
        } else if (key_info == KEY_SCROLLLOCK) {
            scroll_lock = scroll_lock ? false : true;
        } else if (key_info <= 0x7f) {
            key.character = keycode_to_ascii(key_info);
        }
    }

    print_key(key);
}

bool read_ps2_control_reg(uint8_t mask) {
    return inb(PS2_COMMAND_PORT) & mask;
}

int ps2_send_command(uint8_t command, int mode) {
    if (mode == PS2_COMMAND_PORT) {
        while (read_ps2_control_reg(IN_BUFFER_FULL)) {
            outb(PS2_COMMAND_PORT, command);
        }
    } else if (mode == PS2_DATA_PORT) {
        while (read_ps2_control_reg(IN_BUFFER_FULL)) {
            outb(PS2_DATA_PORT, command);
        }
    } else {
        return EINVARG;
    }
    return 0;
}

void ps2_init() {
    kprint("[KERNEL] PS2 Initializing... ");
    while (read_ps2_control_reg(OUT_BUFFER_FULL)) {
        inb(PS2_DATA_PORT);
    }
    ps2_send_command(ENABLE_SCANNING, PS2_COMMAND_PORT);
    unmask_irq(KEYBOARD_IRQ);
    kprint("Success\n");
}