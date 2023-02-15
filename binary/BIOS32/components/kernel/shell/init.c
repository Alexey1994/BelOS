#include "command.c"
#include "input.c"


void start_shell()
{
	open_root();
	
	set_timer_frequency_divider(0, 1193);
	loader_api->set_interrupt_handler((Number)&interrupt_32_handler - 12, 32);
	
	on_key_down_handler = &on_key_down;
	on_key_input_handler = &on_key_input;
	loader_api->set_interrupt_handler((Number)&interrupt_33_handler - 12, 33);
	
	screen_text_color = 10;
	print(">");
	screen_text_color = 15;
}