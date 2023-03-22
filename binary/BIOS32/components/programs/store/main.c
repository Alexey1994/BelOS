#include <API.c>


API* _api;


Number main(Number number_of_arguments, Byte** arguments);


void start(API* api)
{
	get_module_address();
	
	*(API**)((Byte*)&_api + module_address) = api;
	main(api->process.number_of_arguments, api->process.arguments);
}


#include <writer.c>


void print(Byte* parameters, ...)
{
	get_module_address();
	API* api = *(API**)((Byte*)&_api + module_address);
	
	print_in_source(0, api->pipe.write_character, parameters, &parameters + 1);
}


void print_error(Byte* parameters, ...)
{
	get_module_address();
	API* api = *(API**)((Byte*)&_api + module_address);
	
	print_in_source(0, api->display.text.write_character, parameters, &parameters + 1);
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
	API* api = *(API**)((Byte*)&_api + module_address);
	
	if(number_of_arguments < 2) {
		/*print(
			"usage:\n"
			"\tstore LPT1" + module_address
		);*/
		
		Number character;
		
		for(;;) {
			character = api->pipe.read_character(0);
			
			if(character > 255) {
				break;
			}
			
			api->pipe.write_character(0, character);
		}
	}
	else if(!compare_null_terminated_bytes(arguments[1], "LPT1" + module_address)) {
		Number character;
		Number LPT_address;
		
		LPT_address = 0x378;
	
		for(;;) {
			character = api->pipe.read_character(0);
			//api->pipe.write_character(0, character);
			
			Byte control_register;
			control_register = in_8(LPT_address + 2);
			out_8(LPT_address, character);
			out_8(LPT_address + 2, control_register | 1);
			api->process.sleep(10);
			out_8(LPT_address + 2, control_register);
		}
	}
}