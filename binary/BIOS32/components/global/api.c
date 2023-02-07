#ifndef API_INCLUDED
#define API_INCLUDED


#include "types.c"
#include "file.c"


typedef struct
{
	Number16 attributes;
	Number8  window_a;
	Number8  window_b;
	Number16 granularity;
	Number16 window_size;
	Number16 segment_a;
	Number16 segment_b;
	Number32 win_func_ptr;
	Number16 pitch;
	Number16 width;
	Number16 height;
	Number8  w_char;
	Number8  y_char;
	Number8  planes;
	Number8  bpp;
	Number8  banks;
	Number8  memory_model;
	Number8  bank_size;
	Number8  image_pages;
	Number8  reserved0;
 
	Number8  red_mask;
	Number8  red_position;
	Number8  green_mask;
	Number8  green_position;
	Number8  blue_mask;
	Number8  blue_position;
	Number8  reserved_mask;
	Number8  reserved_position;
	Number8  direct_color_attributes;
 
	Number32 framebuffer;
	Number32 off_screen_mem_off;
	Number16 off_screen_mem_size;
	Number8  reserved1[206];
}
VESA_Mode_Info;


typedef struct {
	void    (*write_character_in_console) (Byte* console, Number character);
	void    (*print)            (Byte* parameters, ...);
	
	Boolean (*enum_files)       (File_Enumerator* enumerator);
	Boolean (*open_file)        (FAT_Data* file, Byte* name);
	Number  (*read_file_sector) (FAT_Data* file, Byte* sector);
	
	void    (*sleep)            (Number milliseconds);
	void    (*exit)             (Number code);
	
	void    (*reset)            ();
	
	VESA_Mode_Info* (*set_video_mode)   ();
	
	Number number_of_arguments;
	Byte*  arguments[256];
}
API;


#endif//API_INCLUDED