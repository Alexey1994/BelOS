#include <API.c>
#include "API/heap.c"
#include "API/display.c"
#include "API/file.c"
#include "API/keyboard.c"
#include "API/process.c"
#include "API/pipe.c"


void initialize_program_api(API* api)
{
	api->heap.allocate = &allocate_memory;
	api->heap.free     = &free_memory;
	
	api->file.enumerate   = &enum_files;
	api->file.open        = &open_file;
	api->file.read_sector = &read_file_sector;
	
	api->process.get_first = &get_first_process;
	api->process.create = &create_process;
	api->process.switch_to = &switch_to_process;
	api->process.execute = &execute_command;
	api->process.sleep = &sleep;
	api->process.exit  = &exit;
	
	//api->pipe.read = &read_from_pipe;
	//api->pipe.write = &write_in_pipe;
	api->pipe.read_character = &read_character_from_pipe;
	api->pipe.write_character = &write_character_in_pipe;
	
	api->display.pixel.get_number_of_modes     = &get_number_of_VESA_modes;
	api->display.pixel.get_mode_info           = &get_pixel_display_mode_info;
	api->display.pixel.set_mode                = &set_VESA_mode;
	
	api->display.text.get_cursor_position_x    = &get_text_display_cursor_position_x;
	api->display.text.get_cursor_position_y    = &get_text_display_cursor_position_y;
	api->display.text.get_text_color           = &get_text_display_color;
	api->display.text.get_background_color     = &get_text_display_background_color;
	api->display.text.set_mode                 = &set_text_display_mode;
	api->display.text.set_cursor_position      = &set_text_display_cursor_position;
	api->display.text.set_text_color           = &set_text_display_color;
	api->display.text.set_background_color     = &set_text_display_background_color;
	api->display.text.write_character          = &write_character_in_text_display;
	
	api->keyboard.read_character       = &read_character_from_keyboard_with_wait;
	api->keyboard.set_key_down_handler = &set_key_down_handler;
	api->keyboard.set_key_up_handler   = &set_key_up_handler;
	
	api->reset = loader_api->reset;
}