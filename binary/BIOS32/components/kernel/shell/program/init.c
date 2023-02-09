#include <API.c>


Number32 kernel_stack;
Number32 kernel_base;
Number32 program_stack = 128 * 1024 * 1024;


void execute(void(*start)(API* api), API* api)
{
	
	asm("mov %esp, kernel_stack");
	asm("mov %ebp, kernel_base");
	//asm("mov %esp, program_stack");

	start(api);
	asm("return_address:");

	loader_api->set_text_mode();
}


void exit(Number code)
{
	asm("mov kernel_stack, %esp");
	asm("mov kernel_base, %ebp");
	
	asm("jmp return_address");
}


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


void initialize_program_api(API* api)
{
	api->write_character_in_console = &write_character_in_console;
	api->print            = &print;
	api->enum_files       = &enum_files;
	api->open_file        = &open_file;
	api->read_file_sector = &read_file_sector;
	
	api->sleep            = &sleep;
	api->exit             = &exit;
	
	api->reset            = loader_api->reset;

	api->get_number_of_video_modes     = &get_number_of_video_modes;
	api->get_video_mode_width          = &get_video_mode_width;
	api->get_video_mode_pitch          = &get_video_mode_pitch;
	api->get_video_mode_height         = &get_video_mode_height;
	api->get_video_mode_bits_per_pixel = &get_video_mode_bits_per_pixel;
	api->get_video_mode_framebuffer    = &get_video_mode_framebuffer;
	api->set_video_mode                = &set_video_mode;
	api->set_text_mode                 = loader_api->set_text_mode;

	api->set_key_down_handler = &set_key_down_handler;
	api->set_key_up_handler   = &set_key_up_handler;
	
	parse_program_arguments(api, command);
}