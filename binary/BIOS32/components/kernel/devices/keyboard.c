#ifndef KEYBOARD_INCLUDED
#define KEYBOARD_INCLUDED


typedef enum {
	KEY_ESCAPE = 1,
	
	KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL, KEY_BACKSPACE,
	KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_OPEN_SQUARE_BRACKET, KEY_CLOSE_SQUARE_BRACKET, KEY_ENTER,
	KEY_LEFT_CONTROL, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_APOSTROPHE, KEY_BACKTICK,
	KEY_LEFT_SHIFT, KEY_BACKSLASH, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_RIGHT_SHIFT,
	
	KEY_NUMPAD_MUL,
	KEY_LEFT_ALT,
	KEY_SPACE,
	
	KEY_CAPSLOCK,
	KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
	KEY_NUM_LOCK, KEY_SCROLL_LOCK,
	
	KEY_NUMPAD_7, KEY_NUMPAD_8, KEY_NUMPAD_9, KEY_NUMPAD_MINUS,
	KEY_NUMPAD_4, KEY_NUMPAD_5, KEY_NUMPAD_6, KEY_NUMPAD_PLUS,
	KEY_NUMPAD_1, KEY_NUMPAD_2, KEY_NUMPAD_3,
	KEY_NUMPAD_0, KEY_NUMPAD_DOT,
	
	KEY_F11 = 87,
	KEY_F12 = 88,
}
Key_Codes;


typedef enum {
	KEY_NUMPAD_ENTER = 28,
	KEY_RIGHT_CONTROL = 29,
	KEY_PRINT_SCREEN2 = 42,
	KEY_NUMPAD_DIV = 53,
	KEY_PRINT_SCREEN = 55,
	KEY_RIGHT_ALT = 56,
	
	KEY_HOME = 71,
	KEY_ARROW_UP = 72,
	KEY_PAGE_UP = 73,
	KEY_ARROW_LEFT = 75,
	KEY_ARROW_RIGHT = 77,
	KEY_END = 79,
	KEY_ARROW_DOWN = 80,
	KEY_PAGE_DOWN = 81,
	KEY_INSERT = 82,
	KEY_DELETE = 83,
	
	KEY_OS = 91,
	KEY_CONTEXT_MENU = 93,
}
Special_Key_Codes;


typedef enum {
	/*CHAR_BACKSPACE = 8,
	CHAR_TAB = 9,
	CHAR_ENTER = 10,
	CHAR_SHIFT = 16,
	CHAR_CONTROL = 17,
	CHAR_ALT = 18,
	CHAR_CAPS_LOCK = 20,
	CHAR_ESCAPE = 27,
	
	CHAR_F1 = 112,
	CHAR_F2 = 113,
	CHAR_F3 = 114,
	CHAR_F4 = 115,
	CHAR_F5 = 116,
	CHAR_F6 = 117,
	CHAR_F7 = 118,
	CHAR_F8 = 119,
	CHAR_F9 = 120,
	CHAR_F10 = 121,
	CHAR_F11 = 122,
	CHAR_F12 = 123,
	
	CHAR_NUM_LOCK = 144,
	CHAR_SCROLL_LOCK = 145,*/
	
	CHAR_TAB = '\t',
	CHAR_ENTER = '\n',
}
Characters;


Boolean is_special_key = 0;

/*
//клавиатура со включённым NumLock, отключенным CapsLock, отключенным Shift
Byte key_to_char_code[128] = {
	0,

	CHAR_ESCAPE, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', CHAR_BACKSPACE,
	CHAR_TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', CHAR_ENTER,
	CHAR_CONTROL, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	CHAR_SHIFT, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', CHAR_SHIFT,
	
	'*', //55, * on numpad
	
	CHAR_ALT, ' ', //57
	
	CHAR_CAPS_LOCK, //58 CapsLock
	
	CHAR_F1, CHAR_F2, CHAR_F3, CHAR_F4, CHAR_F5, CHAR_F6, CHAR_F7, CHAR_F8, CHAR_F9, CHAR_F10, //68 F10
	
	CHAR_NUM_LOCK, CHAR_SCROLL_LOCK, //70 ScrollLock
	
	//Numpad
	'7', '8', '9', '-',
	'4', '5', '6', '+',
	'1', '2', '3',
	'0', '.', //83
	
	0, //84, ?
	0, //85, ?
	0, //86, ?
	
	CHAR_F11, //87, F11
	CHAR_F12, //88, F12
};

Byte special_key_to_char_code[128] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	CHAR_ENTER, //28 - NumPad Enter
	CHAR_CONTROL, //29 - Right Control
	
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0, //39
	
	0,
	0,
	0, //42 - Print Screen
	0,
	0,
	0,
	0,
	0,
	0,
	0, //49
	
	0,
	0,
	0,
	'/', //53 - NumPad /
	0,
	0, //55 - Print Screen
	CHAR_ALT, //56 - Right Alt
	0,
	0,
	0,
	
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0, //69
	
	0,
	36, //71 - Home
	38, //72 - ArrowTop
	33, //73 - Page Up
	0,
	37, //75 - ArrowLeft
	0,
	39, //77 - ArrowRight
	0,
	35, //79 - End
	
	40, //80 - ArrowDown
	34, //81 - Page Down
	45, //82 - Insert
	46, //83 - Delete
	0,
	0,
	0,
	0,
	0,
	0, //89
	
	0,
	91, //91 - Windows
	0,
	93, //93 - Context Menu
};*/


Byte key_to_char_code[128] = {
	0,

	0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
	CHAR_TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', CHAR_ENTER,
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
	
	'*', //55, * on numpad
	
	0, ' ', //57
	
	0, //58 CapsLock
	
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //68 F10
	
	0, 0, //70 ScrollLock
	
	//Numpad
	'7', '8', '9', '-',
	'4', '5', '6', '+',
	'1', '2', '3',
	'0', '.', //83
};

Byte special_key_to_char_code[128] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	CHAR_ENTER, //28 - NumPad Enter
	0, //29 - Right Control
	
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0, //39
	
	0,
	0,
	0, //42 - Print Screen
	0,
	0,
	0,
	0,
	0,
	0,
	0, //49
	
	0,
	0,
	0,
	'/', //53 - NumPad /
	0,
	0, //55 - Print Screen
	0, //56 - Right Alt
	0,
	0,
	0,
	
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0, //69
	
	0,
	0, //71 - Home
	0, //72 - ArrowTop
	0, //73 - Page Up
	0,
	0, //75 - ArrowLeft
	0,
	0, //77 - ArrowRight
	0,
	0, //79 - End
	
	0, //80 - ArrowDown
	0, //81 - Page Down
	0, //82 - Insert
	0, //83 - Delete
	0,
	0,
	0,
	0,
	0,
	0, //89
	
	0,
	0, //91 - Windows
	0,
	0, //93 - Context Menu
};


Byte key_to_shifted_char_code[128] = {
	0,

	0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
	0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', CHAR_ENTER,
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
	0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	
	'*', //55, * on numpad
	
	0, ' ', //57
	
	0, //58 CapsLock
	
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //68 F10
	
	0, 0, //70 ScrollLock
	
	//Numpad
	0, 0, 0, '-',
	0, 0, 0, '+',
	0, 0, 0,
	0, 0, //83
};


void(*on_key_down_handler)(Byte key_code, Boolean is_special) = 0;
void(*on_key_up_handler)(Byte key_code, Boolean is_special) = 0;
void(*on_key_input_handler)(Byte char_code) = 0;


Boolean pressed_shift_key = 0;


asm (
	"pusha \n"
	"call interrupt_33_handler\n"
	"popa \n"
	"mov $0x20, %al \n"
	"out %al, $0x20 \n"
	"iret"
);
void interrupt_33_handler()
{
	Byte key_state;
	Byte key_code;
	Boolean is_up;
	Byte char_code;
	
	key_state = in_8(0x60);
	
	char_code = 0;

	if(key_state == 0xE0) {
		is_special_key = 1;
	}
	else {
		key_code = key_state & 0b1111111;
		is_up = key_state & 0b10000000;
		
		if(key_code == KEY_LEFT_SHIFT || key_code == KEY_RIGHT_SHIFT) {
			pressed_shift_key = !is_up;
		}
		
		if(is_up) {
			if(on_key_up_handler) {
				on_key_up_handler(key_code, is_special_key);
			}
			
			if(is_special_key) {
				is_special_key = 0;
			}
			else {
				
			}
		}
		else {
			if(on_key_down_handler) {
				on_key_down_handler(key_code, is_special_key);
			}
			
			if(is_special_key) {
				char_code = special_key_to_char_code[key_code];
				is_special_key = 0;
			}
			else {
				if(pressed_shift_key) {
					char_code = key_to_shifted_char_code[key_code];
				}
				else {
					char_code = key_to_char_code[key_code];
				}
			}
		}
		
		if(on_key_input_handler && char_code) {
			on_key_input_handler(char_code);	
		}
	}
}


#endif//KEYBOARD_INCLUDED