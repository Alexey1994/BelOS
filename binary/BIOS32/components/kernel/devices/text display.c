#ifndef DISPLAY_INCLUDED
#define DISPLAY_INCLUDED


#define TEXT_DISPLAY_WIDTH 80
#define TEXT_DISPLAY_HEIGHT 25
//#define TEXT_DISPLAY_HEIGHT 50


Number cursor_pos_x = 0;
Number cursor_pos_y = TEXT_DISPLAY_HEIGHT - 1;


void hide_text_display_cursor()
{
	out_8(0x3D4, 0x0A);
	out_8(0x3D5, 0x20);
}


Number get_text_display_cursor_position_x()
{
	return cursor_pos_x;
}


Number get_text_display_cursor_position_y()
{
	return cursor_pos_y;
}


void set_text_display_cursor_position(Number x, Number y)
{
	Number16 position;
	
	cursor_pos_x = x;
	cursor_pos_y = y;
	
	position = y * TEXT_DISPLAY_WIDTH + x;
	
	out_8(0x3D4, 0x0F);
	out_8(0x3D5, position);
	
	out_8(0x3D4, 0x0E);
	out_8(0x3D5, position >> 8);
}


Byte text_display_color = 15;
Byte text_display_background_color = 0;


Number get_text_display_color()
{
	return text_display_color;
}


Number get_text_display_background_color()
{
	return text_display_background_color;
}


void set_text_display_color(Number color)
{
	text_display_color = color;
}


void set_text_display_background_color(Number color)
{
	text_display_background_color = color;
}


void set_character_in_text_display(Number16* display, Number x, Number y, Number character)
{
	if(!display) {
		display = 0xB8000;
	}
	
	display[y * TEXT_DISPLAY_WIDTH + x] = (Byte)character
		+ (text_display_color << 8)
		+ (text_display_background_color << 12);
}


void scroll_text_display_down(Number16* display)
{
	Number x;
	Number y;
	
	if(!display) {
		display = 0xB8000;
	}
	
	for(y = 0; y < TEXT_DISPLAY_HEIGHT - 1; ++y) {
		for(x = 0; x < TEXT_DISPLAY_WIDTH; ++x) {
			display[y * TEXT_DISPLAY_WIDTH + x] = display[(y + 1) * TEXT_DISPLAY_WIDTH + x];
		}
	}
		
	for(x = 0; x < TEXT_DISPLAY_WIDTH; ++x) {
		display[(TEXT_DISPLAY_HEIGHT - 1) * TEXT_DISPLAY_WIDTH + x] = (15 << 8);
	}
}


void write_character_in_text_display(Number16* display, Number character)
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
			write_character_in_text_display(display, ' ');
			write_character_in_text_display(display, ' ');
			write_character_in_text_display(display, ' ');
			write_character_in_text_display(display, ' ');
			break;
		}
		
		default: {
			set_character_in_text_display(display, cursor_pos_x, cursor_pos_y, character);
			++cursor_pos_x;
		}
	}
	
	if(cursor_pos_x >= TEXT_DISPLAY_WIDTH) {
		cursor_pos_x = 0;
		++cursor_pos_y;
	}
	
	if(cursor_pos_y >= TEXT_DISPLAY_HEIGHT) {
		--cursor_pos_y;
		
		scroll_text_display_down(display);
	}
	
	set_text_display_cursor_position(cursor_pos_x, cursor_pos_y);
}


#include <writer.c>


void print(Byte* parameters, ...)
{
	print_in_source(0, &write_character_in_text_display, parameters, &parameters + 1);
}


#endif//DISPLAY_INCLUDED