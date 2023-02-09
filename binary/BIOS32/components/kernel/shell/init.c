Loader_Api* loader_api;

Byte command[256];
Number command_size = 0;


void set_key_down_handler(void(*key_down_handler)(Byte key_code, Boolean is_special))
{
	on_key_down_handler = key_down_handler;
}


void set_key_up_handler(void(*key_up_handler)(Byte key_code, Boolean is_special))
{
	on_key_up_handler = key_up_handler;
}


#include "file.c"
#include "video.c"
#include "program/init.c"
#include "program/COM.c"
#include "program/EXE.c"
#include "command.c"
#include "input.c"


void start_shell(Loader_Api* api)
{
	loader_api = api;
	
	
	/*VESA_Info*     info;
	Number16*      modes;
	VESA_Mode_Info mode_info;
	Number         i;
	
	info = loader_api->get_VESA_Info();
	modes = info->video_modes;
	
	for(i = 0; modes[i] != 0xFFFF; ++i) {
		loader_api->get_VESA_mode_info(&mode_info, modes[i] | 0x4000);
		print("%d %d %d\n", mode_info.width, mode_info.height, mode_info.bpp);
	}
	
	loader_api->set_VESA_mode(modes[0] | 0x4000);
	
	return;*/
	
	
	open_root();
	
	on_key_down_handler = &on_key_down;
	on_key_input_handler = &on_key_input;
	
	set_timer_frequency_divider(0, 1193);
	api->set_interrupt_handler((Number)&interrupt_32_handler - 12, 32);
	api->set_interrupt_handler((Number)&interrupt_33_handler - 12, 33);
	
	console_text_color = 10;
	print(">");
	console_text_color = 15;
}