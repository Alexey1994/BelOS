#ifndef API_INCLUDED
#define API_INCLUDED


#include "types.c"
#include "keyboard.c"
#include "file.c"


typedef struct {
	void    (*write_character_in_console) (Byte* console, Number character);
	void    (*print)            (Byte* parameters, ...);
	
	Boolean (*enum_files)       (File_Enumerator* enumerator);
	Boolean (*open_file)        (FAT_Data* file, Byte* name);
	Number  (*read_file_sector) (FAT_Data* file, Byte* sector);
	
	void    (*sleep)            (Number milliseconds);
	void    (*exit)             (Number code);
	
	void    (*reset)            ();
	
	//void* (*get_video_mode)   (Number index);
	//void  (*set_video_mode)   (Number index);
	
	Number  (*get_number_of_video_modes)     ();
	Number  (*get_video_mode_width)          (Number video_mode_index);
	Number  (*get_video_mode_pitch)          (Number video_mode_index);
	Number  (*get_video_mode_height)         (Number video_mode_index);
	Number  (*get_video_mode_bits_per_pixel) (Number video_mode_index);
	void*   (*get_video_mode_framebuffer)    (Number video_mode_index);
	Boolean (*set_video_mode)                (Number video_mode_index);
	void    (*set_text_mode)                 ();
	
	void (*set_key_down_handler) (void(*key_down_handler)(Byte key_code, Boolean is_special));
	void (*set_key_up_handler) (void(*key_up_handler)(Byte key_code, Boolean is_special));
	
	Number number_of_arguments;
	Byte*  arguments[256];
}
API;


#endif//API_INCLUDED