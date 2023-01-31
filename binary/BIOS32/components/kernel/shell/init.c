Loader_Api* loader_api;

Byte command[256];
Number command_size = 0;


#include "file.c"
#include "program/api.c"
#include "program/COM.c"
#include "program/EXE.c"
#include "command.c"
#include "input.c"


void start_shell(Loader_Api* api)
{
	loader_api = api;
	
	open_root();
	
	on_key_down_handler = &on_key_down;
	on_key_input_handler = &on_key_input;
	
	api->set_interrupt_handler((Number)&interrupt_32_handler - 12, 32);
	api->set_interrupt_handler((Number)&interrupt_33_handler - 12, 33);
	
	console_text_color = 10;
	print(">");
	console_text_color = 15;
}