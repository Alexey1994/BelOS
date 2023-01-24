#include "file.c"
#include "program/COM.c"
#include "program/EXE.c"
#include "command.c"
#include "input.c"


void start_shell()
{
	on_key_input_handler = &on_key_input;
	on_key_down_handler = &on_key_down;
	
	open_FAT_File_System(&fs, read_sector);

	set_interrupt_handler((Number)&interrupt_32_handler - 12, 32);
	set_interrupt_handler((Number)&interrupt_33_handler - 12, 33);

	console_text_color = 10;
	print(">");
	console_text_color = 15;
}