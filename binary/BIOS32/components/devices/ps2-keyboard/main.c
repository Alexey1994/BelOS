#include <API.c>


API* _api;

Pipe_Interface* _pipe_interface;
Text_Display_Interface* _text_display_interface;
Interrupt_Interface* _interrupt_interface;
Process_Interface* _process_interface;


Number main(Number number_of_arguments, Byte** arguments);


void print_error(Byte* parameters, ...);


void start(API* api)
{
	get_module_address_by_function(start);

	global(_api) = api;
	
	global(_pipe_interface) = api->get("pipe" + module_address);
	global(_text_display_interface) = api->get("display/text" + module_address);
	global(_interrupt_interface) = api->get("interrupt" + module_address);
	global(_process_interface) = api->get("process" + module_address);
	
	main(api->number_of_arguments, api->arguments);
}


#include <memory.c>
#include <writer.c>


void print(Byte* parameters, ...)
{
	get_module_address_by_function(print);
	Pipe_Interface* pipe_interface = global(_pipe_interface);
	
	print_in_source(0, pipe_interface->write_character, parameters, &parameters + 1);
}


void print_error(Byte* parameters, ...)
{
	get_module_address_by_function(print_error);
	Text_Display_Interface* text_display_interface = global(_text_display_interface);
	
	print_in_source(0, text_display_interface->write_character, parameters, &parameters + 1);
}




#include <keyboard.c>


typedef struct {
	Keyboard_Interface interface;
	
	void (*on_key_down_handler)(Byte key_code, Boolean is_special);
	void (*on_key_up_handler)(Byte key_code, Boolean is_special);
	void (*on_key_input_handler)(Byte char_code);

	Boolean is_special_key;
	Boolean pressed_shift_key;
	Boolean pressed_control_key;
	Byte    last_character;
	Byte    key_to_char_code[128];
	Byte    key_to_shifted_char_code[128];
	Byte    special_key_to_char_code[128];
}
Keyboard;

Keyboard _keyboard = {	
	.on_key_down_handler = 0,
	.on_key_up_handler = 0,
	.on_key_input_handler = 0,
	
	.is_special_key = 0,
	.pressed_shift_key = 0,
	.last_character = 0,
	
	.key_to_char_code = {
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
	},
	
	.key_to_shifted_char_code = {
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
	},
	
	.special_key_to_char_code = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		
		0, 0, 0, 0, 0, 0, 0, 0,
		CHAR_ENTER, //28 - NumPad Enter
		0, //29 - Right Control
		
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //39
		
		0, 0,
		0, /*42 - Print Screen*/
		0, 0, 0, 0, 0, 0, 0, //49
		
		0, 0, 0,
		'/', //53 - NumPad /
		0,
		0, //55 - Print Screen
		0, //56 - Right Alt
		0, 0, 0,
		
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //69
		
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
		0, 0, 0, 0, 0, 0, //89
		
		0,
		0, //91 - Windows
		0,
		0, //93 - Context Menu
	}
};


Byte read_character_from_keyboard()
{
	get_module_address_by_function(read_character_from_keyboard);
	Keyboard* keyboard = global_ptr(_keyboard);
	
	
	Byte result_character;
	
	result_character = keyboard->last_character;
	keyboard->last_character = 0;
	
	return result_character;
}


#include <IO.c>


asm("pusha");
asm("call . + 11");
asm("mov $0x20, %al");
asm("out %al, $0x20");
asm("popa");
asm("iret");
void interrupt_33_handler()
{
	get_module_address_by_function(interrupt_33_handler);
	Interrupt_Interface* interrupt_interface = global(_interrupt_interface);
	Keyboard* keyboard = global_ptr(_keyboard);
	Process_Interface* process_interface = global(_process_interface);
	
	
	Byte key_state;
	Byte key_code;
	Boolean is_up;
	Byte char_code;
	
	key_state = in_8(0x60);
	
	char_code = 0;

	if(key_state == 0xE0) {
		keyboard->is_special_key = 1;
	}
	else {
		key_code = key_state & 0b1111111;
		is_up = key_state & 0b10000000;
		
		if(key_code == KEY_LEFT_SHIFT || key_code == KEY_RIGHT_SHIFT) {
			keyboard->pressed_shift_key = !is_up;
		}
		else if(key_code == KEY_LEFT_CONTROL) {
			keyboard->pressed_control_key = !is_up;
		}
		else if(key_code == KEY_C) {
			if(keyboard->pressed_control_key) {
				process_interface->exit(1);
			}
		}
		
		if(is_up) {
			if(keyboard->on_key_up_handler) {
				keyboard->on_key_up_handler(key_code, keyboard->is_special_key);
			}
			
			if(keyboard->is_special_key) {
				keyboard->is_special_key = 0;
			}
			else {
				
			}
		}
		else {
			if(keyboard->on_key_down_handler) {
				keyboard->on_key_down_handler(key_code, keyboard->is_special_key);
			}
			
			if(keyboard->is_special_key) {
				char_code = keyboard->special_key_to_char_code[key_code];
				keyboard->is_special_key = 0;
			}
			else {
				if(keyboard->pressed_shift_key) {
					char_code = keyboard->key_to_shifted_char_code[key_code];
				}
				else {
					char_code = keyboard->key_to_char_code[key_code];
				}
			}
		}
		
		if(char_code) {
			keyboard->last_character = char_code;
		}
		
		if(keyboard->on_key_input_handler && char_code) {
			keyboard->on_key_input_handler(char_code);
		}
	}
}


Number read_character_with_wait(Byte* keyboard)
{
	//get_module_address_by_function(read_character_with_wait);
	//Keyboard* keyboard = global_ptr(_keyboard);
	
	
	Number character;
	
	for(;;) {
		character = read_character_from_keyboard();
		
		if(character) {	
			break;
		}
		
		asm("hlt");
	}
	
	return character;
}


void set_key_down_handler(void(*key_down_handler)(Byte key_code, Boolean is_special))
{
	get_module_address_by_function(set_key_down_handler);
	Keyboard* keyboard = global_ptr(_keyboard);
	
	
	keyboard->on_key_down_handler = key_down_handler;
}


void set_key_up_handler(void(*key_up_handler)(Byte key_code, Boolean is_special))
{
	get_module_address_by_function(set_key_up_handler);
	Keyboard* keyboard = global_ptr(_keyboard);
	
	
	keyboard->on_key_up_handler = key_up_handler;
}


Number main(Number number_of_arguments, Byte** arguments)
{
	get_module_address_by_function(main);
	Interrupt_Interface* interrupt_interface = global(_interrupt_interface);
	Keyboard* keyboard = global_ptr(_keyboard);
	API* api = global(_api);
	
	
	keyboard->interface.read_character       = global_ptr(read_character_with_wait);
	keyboard->interface.set_key_down_handler = global_ptr(set_key_down_handler);
	keyboard->interface.set_key_up_handler   = global_ptr(set_key_up_handler);
	api->create("/keyboard" + module_address, &keyboard->interface);
	
	interrupt_interface->set(global_ptr(interrupt_33_handler) - 12, 33);
}