#include <API.c>
#include "API/heap.c"
#include "API/video.c"
#include "API/file.c"
#include "API/keyboard.c"
#include "API/console.c"
#include "API/process.c"
#include "API/pipe.c"


void initialize_program_api(API* api)
{
	api->process.sleep = &sleep;
	api->process.exit  = &exit;
	
	//api->pipe.read = &read_from_pipe;
	//api->pipe.write = &write_in_pipe;
	api->pipe.read_character = &read_character_from_pipe;
	api->pipe.write_character = &write_character_in_pipe;
	
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
}


#include "COM.c"
#include "EXE.c"
#include "ELF.c"