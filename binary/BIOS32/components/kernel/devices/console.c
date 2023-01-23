#ifndef CONSOLE_INCLUDED
#define CONSOLE_INCLUDED


#include <writer.c>


void hide_console_cursor()
{
	out_8(0x3D4, 0x0A);
	out_8(0x3D5, 0x20);
}


void set_cursor_position(Number x, Number y)
{
	Number16 position;
	
	position = y * 80 + x;
	
	out_8(0x3D4, 0x0F);
	out_8(0x3D5, position);
	
	out_8(0x3D4, 0x0E);
	out_8(0x3D5, position >> 8);
}


Byte console_text_color = 15;
Byte console_background_color = 0;


void set_character_in_console(Byte* console, Number x, Number y, Number character)
{
	Number16* screen;
	
	if(!console) {
		screen = 0xB8000;
	}
	
	screen[y * 80 + x] = (Byte)character
		+ (console_text_color << 8)
		+ (console_background_color << 12);
}


void scroll_console(Byte* console)
{
	Number16* screen;
	Number    x;
	Number    y;
	
	if(!console) {
		screen = 0xB8000;
	}
	
	for(y = 0; y < 24; ++y) {
		for(x = 0; x < 80; ++x) {
			screen[y * 80 + x] = screen[(y + 1) * 80 + x];
		}
	}
		
	for(x = 0; x < 80; ++x) {
		screen[24 * 80 + x] = (15 << 8);
	}
}


Number cursor_pos_x = 0;
Number cursor_pos_y = 24;


void write_character_in_console(Byte* console, Number character)
{
	switch(character) {
		case '\n': {
			cursor_pos_x = 0;
			++cursor_pos_y;
			break;
		}
		
		case '\r': {
			cursor_pos_x = 0;
			break;
		}
		
		case '\t': {
			write_character_in_console(console, ' ');
			write_character_in_console(console, ' ');
			write_character_in_console(console, ' ');
			write_character_in_console(console, ' ');
			break;
		}
		
		default: {
			set_character_in_console(console, cursor_pos_x, cursor_pos_y, character);
			++cursor_pos_x;
		}
	}
	
	if(cursor_pos_x >= 80) {
		cursor_pos_x = 0;
		++cursor_pos_y;
	}
	
	if(cursor_pos_y >= 25) {
		--cursor_pos_y;
		
		scroll_console(console);
	}
	
	set_cursor_position(cursor_pos_x, cursor_pos_y);
}


void print(Byte* parameters, ...)
{
	print_in_source(0, &write_character_in_console, parameters, &parameters + 1);
}


#endif//CONSOLE_INCLUDED