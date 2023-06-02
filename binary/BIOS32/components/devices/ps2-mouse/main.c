#include <API.c>


API* _api;

Pipe_Interface* _pipe_interface;
File_Interface* _file_interface;
Text_Display_Interface* _text_display_interface;
Interrupt_Interface* _interrupt_interface;


Number main(Number number_of_arguments, Byte** arguments);


void print_error(Byte* parameters, ...);


void start(API* api)
{
	get_module_address_by_function(start);

	global(_api) = api;
	
	global(_pipe_interface) = api->get("pipe" + module_address);
	global(_file_interface) = api->get("file" + module_address);
	global(_text_display_interface) = api->get("display/text" + module_address);
	global(_interrupt_interface) = api->get("interrupt" + module_address);
	
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




typedef struct {
	Mouse_Interface interface;
	Byte            packet[3];
	Number          packet_index;
	void          (*handler)(Mouse_Event* event);
}
Mouse;


Mouse _mouse;


typedef enum {
	PS2_OUTPUT_BUFFER_FULL       = 0b00000001,
	PS2_INPUT_BUFFER_FULL        = 0b00000010,
	PS2_INITIALIZED              = 0b00000100,
	PS2_A2_STATE                 = 0b00001000,
	PS2_KEYBOARD_CONNECTED       = 0b00010000,
	PS2_MOUSE_OUTPUT_BUFFER_FULL = 0b00100000,
	PS2_TIMEOUT_ERROR            = 0b01000000,
	PS2_PARITY_ERROR             = 0b10000000
}
PS2_State;


typedef enum {
	PS2_READ_COMMAND_BYTE          = 0x20,
	PS2_WRITE_COMMAND_BYTE         = 0x60,
	PS2_DISABLE_MOUSE_INTERFACE    = 0xA7,
	PS2_ENABLE_MOUSE_INTERFACE     = 0xA8,
	PS2_MOUSE_INTERFACE_TEST       = 0xA9,
	PS2_KEYBOARD_INTERFACE_TEST    = 0xAB,
	PS2_DISABLE_KEYBOARD_INTERFACE = 0xAD,
	PS2_ENABLE_KEYBOARD_INTERFACE  = 0xAE,
	PS2_WRITE_IN_MOUSE_DEVICE      = 0xD4,
}
PS2_Command;


typedef enum {
	PS2_ENABLE_INPUT_BUFFER_FULL_INTERRUPT       = 0b00000001, //IRQ1
	PS2_ENABLE_MOUSE_INPUT_BUFFER_FULL_INTERRUPT = 0b00000010, //IRQ12
	PS2_DISABLE_MOUSE                            = 0b00100000,
}
PS2_Command_Byte;


typedef enum {
	PS2_MOUSE_ENABLE_STREAMING  = 0xF4,
	PS2_MOUSE_DISABLE_STREAMING = 0xF5,
	PS2_MOUSE_SET_DEFAULTS      = 0xF6,
}
PS2_Mouse_Device_Command;


#include <IO.c>


Byte read_data_from_ps2()
{
	Number tries_count;
	
	//wait while input buffer not full
	tries_count = 0;
	while(!(in_8(0x64) & PS2_MOUSE_OUTPUT_BUFFER_FULL)) {
		++tries_count;
		
		if(tries_count > 1000) {
			return 0;
		}
	}
	
	return in_8(0x60);
}


Boolean write_data_in_ps2(Byte data)
{
	Number tries_count;
	
	//wait while output buffer not empty
	tries_count = 0;
	while(in_8(0x64) & PS2_OUTPUT_BUFFER_FULL) {
		++tries_count;
		
		if(tries_count > 1000) {
			return 0;
		}
	}
	
	out_8(0x60, data);
	
	return 1;
}


Boolean ps2_mouse_acknowledge()
{
	return read_data_from_ps2() == 0xFA;
}


asm("pusha");
asm("call . + 13");
asm("mov $0x20, %al");
asm("out %al, $0xA0");
asm("out %al, $0x20");
asm("popa");
asm("iret");
void interrupt_44_handler()
{
	get_module_address_by_function(interrupt_44_handler);
	Mouse* mouse = global_ptr(_mouse);
	
	
	mouse->packet[mouse->packet_index] = read_data_from_ps2();
	++mouse->packet_index;
	
	if(mouse->packet_index == 3) {
		//print_error(
		//	"lb%d "
		//	"rb%d "
		//	"\n" + module_address,
		//	!!(mouse->packet[0] & 0b00000001),
		//	!!(mouse->packet[0] & 0b00000010)
		//);
		
		if(mouse->handler) {
			mouse->handler(mouse->packet);
		}
		
		mouse->packet_index = 0;
	}
}


Boolean initialize_ps2_mouse()
{
	get_module_address_by_function(initialize_ps2_mouse);
	Interrupt_Interface* interrupt_interface = global(_interrupt_interface);
	Mouse* mouse = global_ptr(_mouse);
	
	
	Byte command;
	
	out_8(0x64, PS2_DISABLE_MOUSE_INTERFACE);
	
	//reset mouse
	out_8(0x64, PS2_WRITE_IN_MOUSE_DEVICE);
	if(!write_data_in_ps2(PS2_MOUSE_SET_DEFAULTS)) {
		goto error;
	}
	if(!ps2_mouse_acknowledge()) {
		goto error;
	}
	
	//enable streaming
	out_8(0x64, PS2_WRITE_IN_MOUSE_DEVICE);
	if(!write_data_in_ps2(PS2_MOUSE_ENABLE_STREAMING)) {
		goto error2;
	}
	if(!ps2_mouse_acknowledge()) {
		goto error2;
	}
	
	//enable IRQ12
	asm("cli");
	interrupt_interface->set(global_ptr(interrupt_44_handler) - 14, 44);
	mouse->packet_index = 0;
	out_8(0x64, PS2_READ_COMMAND_BYTE);
	command = in_8(0x60);
	command |= PS2_ENABLE_MOUSE_INPUT_BUFFER_FULL_INTERRUPT;
	command &= ~PS2_DISABLE_MOUSE;
	out_8(0x64, PS2_WRITE_COMMAND_BYTE);
	write_data_in_ps2(command);
	asm("sti");
	
	
	out_8(0x64, PS2_ENABLE_MOUSE_INTERFACE);
	
	return 1;
	
	error2: {
		interrupt_interface->clean(44);
	}
	
	error: {
		return 0;
	}
}


void set_mouse_handler(void (*mouse_handler)(Mouse_Event* event))
{
	get_module_address_by_function(set_mouse_handler);
	Mouse* mouse = global_ptr(_mouse);
	
	
	mouse->handler = mouse_handler;
}


Number main(Number number_of_arguments, Byte** arguments)
{
	get_module_address_by_function(main);
	Mouse* mouse = global_ptr(_mouse);
	API* api = global(_api);
	
	
	if(initialize_ps2_mouse()) {
		mouse->interface.set_handler = global_ptr(set_mouse_handler);
		api->create("/mouse" + module_address, &mouse->interface);
	}
}