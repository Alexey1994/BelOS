#include <API.c>
#include "API/video.c"
#include "API/file.c"
#include "API/keyboard.c"
#include "API/process.c"
#include "API/console.c"


void parse_program_arguments(API* api, Byte* command)
{
	api->process.number_of_arguments = 0;	

	while(*command && api->process.number_of_arguments < sizeof(api->process.arguments) / sizeof(api->process.arguments[0])) {
		api->process.arguments[api->process.number_of_arguments] = command;
		++api->process.number_of_arguments;
		
		while(*command && *command != ' ') {
			++command;
		}
		
		if(*command == ' ') {
			*command = '\0';
			++command;
		}
		
		while(*command && *command == ' ') {
			++command;
		}
	}
}


void initialize_program_api(API* api, Byte* command)
{
	api->process.sleep = &sleep;
	api->process.exit  = &exit;
	
	api->file.enumerate   = &enum_files;
	api->file.open        = &open_file;
	api->file.read_sector = &read_file_sector;
	
	api->console.read_character  = &read_character_from_console;
	api->console.write_character = &write_character_in_screen;
	api->console.print           = &print;
	
	api->keyboard.set_key_down_handler = &set_key_down_handler;
	api->keyboard.set_key_up_handler   = &set_key_up_handler;
	
	api->screen.get_number_of_video_modes     = &get_number_of_video_modes;
	api->screen.get_video_mode_width          = &get_video_mode_width;
	api->screen.get_video_mode_pitch          = &get_video_mode_pitch;
	api->screen.get_video_mode_height         = &get_video_mode_height;
	api->screen.get_video_mode_bits_per_pixel = &get_video_mode_bits_per_pixel;
	api->screen.get_video_mode_framebuffer    = &get_video_mode_framebuffer;
	api->screen.set_video_mode                = &set_video_mode;
	api->screen.set_text_mode                 = &set_text_mode;
	
	api->reset = loader_api->reset;
	
	parse_program_arguments(api, command);
}


#include "COM.c"
#include "EXE.c"
#include "ELF.c"