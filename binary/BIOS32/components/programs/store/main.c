#include <API.c>


API* _api;

Pipe_Interface* _pipe_interface;
Process_Interface* _process_interface;
Text_Display_Interface* _text_display_interface;


Number main(Number number_of_arguments, Byte** arguments);


void start(API* api)
{
	get_module_address();
	
	global(_api) = api;
	
	global(_pipe_interface) = api->get("pipe" + module_address);
	global(_process_interface) = api->get("process" + module_address);
	global(_text_display_interface) = api->get("display/text" + module_address);
	
	main(api->number_of_arguments, api->arguments);
}


#include <writer.c>


void print(Byte* parameters, ...)
{
	get_module_address();
	Pipe_Interface* pipe_interface = global(_pipe_interface);
	
	print_in_source(0, pipe_interface->write_character, parameters, &parameters + 1);
}


void print_error(Byte* parameters, ...)
{
	get_module_address();
	Text_Display_Interface* text_display_interface = global(_text_display_interface);
	
	print_in_source(0, text_display_interface->write_character, parameters, &parameters + 1);
}


#include <memory.c>


Number8 in_8(Number16 port)
{
	Number8 data;

	asm volatile(
		"inb %1, %0"
		: "=a"(data)
		: "Nd"(port)
	);

	return data;
}


void out_8(Number16 port, Number8 data)
{
	asm volatile(
		"outb %0, %1"
		:
		: "a"(data), "Nd"(port)
	);
}


Number main(Number number_of_arguments, Byte** arguments)
{
	get_module_address();
	API* api = global(_api);
	Pipe_Interface* pipe_interface = global(_pipe_interface);
	Process_Interface* process_interface = global(_process_interface);
	
	
	if(number_of_arguments < 2) {
		/*print(
			"usage:\n"
			"\tstore LPT1" + module_address
		);*/
		
		Number character;
		
		for(;;) {
			character = pipe_interface->read_character(0);
			
			if(character > 255) {
				break;
			}
			
			pipe_interface->write_character(0, character);
		}
	}
	else if(!compare_null_terminated_bytes(arguments[1], "LPT1" + module_address)) {
		Number character;
		Number LPT_address;
		
		LPT_address = 0x378;
	
		for(;;) {
			character = pipe_interface->read_character(0);
			//pipe_interface->write_character(0, character);
			
			Byte control_register;
			control_register = in_8(LPT_address + 2);
			out_8(LPT_address, character);
			out_8(LPT_address + 2, control_register | 1);
			process_interface->sleep(10);
			out_8(LPT_address + 2, control_register);
		}
	}
}