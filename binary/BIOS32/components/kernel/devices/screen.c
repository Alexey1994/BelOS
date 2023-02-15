#ifndef SCREEN_INCLUDED
#define SCREEN_INCLUDED


#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 50


void hide_console_cursor()
{
	out_8(0x3D4, 0x0A);
	out_8(0x3D5, 0x20);
}


void set_cursor_position(Number x, Number y)
{
	Number16 position;
	
	position = y * SCREEN_WIDTH + x;
	
	out_8(0x3D4, 0x0F);
	out_8(0x3D5, position);
	
	out_8(0x3D4, 0x0E);
	out_8(0x3D5, position >> 8);
}


Byte screen_text_color = 15;
Byte screen_background_color = 0;


void set_character_in_screen(Number16* screen, Number x, Number y, Number character)
{
	if(!screen) {
		screen = 0xB8000;
	}
	
	screen[y * SCREEN_WIDTH + x] = (Byte)character
		+ (screen_text_color << 8)
		+ (screen_background_color << 12);
}


Number cursor_pos_x = 0;
Number cursor_pos_y = SCREEN_HEIGHT - 1;


void scroll_screen_down(Number16* screen)
{
	Number x;
	Number y;
	
	if(!screen) {
		screen = 0xB8000;
	}
	
	for(y = 0; y < SCREEN_HEIGHT - 1; ++y) {
		for(x = 0; x < SCREEN_WIDTH; ++x) {
			screen[y * SCREEN_WIDTH + x] = screen[(y + 1) * SCREEN_WIDTH + x];
		}
	}
		
	for(x = 0; x < SCREEN_WIDTH; ++x) {
		screen[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + x] = (15 << 8);
	}
}


void write_character_in_screen(Number16* screen, Number character)
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
			write_character_in_screen(screen, ' ');
			write_character_in_screen(screen, ' ');
			write_character_in_screen(screen, ' ');
			write_character_in_screen(screen, ' ');
			break;
		}
		
		default: {
			set_character_in_screen(screen, cursor_pos_x, cursor_pos_y, character);
			++cursor_pos_x;
		}
	}
	
	if(cursor_pos_x >= SCREEN_WIDTH) {
		cursor_pos_x = 0;
		++cursor_pos_y;
	}
	
	if(cursor_pos_y >= SCREEN_HEIGHT) {
		--cursor_pos_y;
		
		scroll_screen_down(screen);
	}
	
	set_cursor_position(cursor_pos_x, cursor_pos_y);
}


#include <writer.c>


void print(Byte* parameters, ...)
{
	print_in_source(0, &write_character_in_screen, parameters, &parameters + 1);
}


#endif//SCREEN_INCLUDED