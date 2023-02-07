#include <API.c>


Number32 kernel_stack;
Number32 program_stack = 128 * 1024 * 1024;


void parse_program_arguments(API* api, Byte* command_line)
{
	api->number_of_arguments = 0;	

	while(*command_line && api->number_of_arguments < sizeof(api->arguments) / sizeof(api->arguments[0])) {
		api->arguments[api->number_of_arguments] = command_line;
		++api->number_of_arguments;
		
		while(*command_line && *command_line != ' ') {
			++command_line;
		}
		
		if(*command_line == ' ') {
			*command_line = '\0';
			++command_line;
		}
		
		while(*command_line && *command_line == ' ') {
			++command_line;
		}
	}
}


Number return_address;

void exit(Number code)
{
	//asm("jmp return_address")
	
	for(;;) {
		asm("hlt");
	}
}


void execute(void(*start)(API* api), API* api)
{
	//asm("mov %esp, kernel_stack");
	//asm("mov %esp, program_stack");
	//print("%d\n", kernel_stack);
	start(api);
	//asm("mov kernel_stack, %esp");
}


void initialize_program_api(API* api)
{
	api->write_character_in_console = &write_character_in_console;
	api->print            = &print;
	api->enum_files       = &enum_files;
	api->open_file        = &open_file;
	api->read_file_sector = &read_file_sector;
	
	api->sleep            = &sleep;
	api->exit             = &exit;
	
	api->set_video_mode   = loader_api->set_video_mode;
	
	api->reset            = loader_api->reset;
	
	parse_program_arguments(api, command);
}