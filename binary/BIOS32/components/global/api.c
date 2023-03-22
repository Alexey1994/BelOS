#ifndef API_INCLUDED
#define API_INCLUDED


#include "types.c"
#include "file.c"
#include "display.c"
#include "keyboard.c"


#define MAX_NUMBER_OF_ARGUMENTS 256


typedef struct {
	struct {
		void* (*allocate) (Number block_size);
		void  (*free)     (void* block);
	}
	heap;
	
	struct {
		Boolean (*enumerate)   (File_Enumerator* enumerator);
		Boolean (*open)        (FAT_Data* file, Byte* name);
		Number  (*read_sector) (FAT_Data* file, Byte* sector);
	}
	file;
	
	struct {
		struct Process* (*get_first) ();
		struct Process* (*create)    (Byte* command, struct Process* previous_piping_process);
		void            (*switch_to) (struct Process* process);
		Boolean         (*execute)   (Byte* command);
		void            (*sleep)     (Number milliseconds);
		void            (*exit)      (Number code);
		
		Byte   command[256];
		Number number_of_arguments;
		Byte*  arguments[MAX_NUMBER_OF_ARGUMENTS];
	}
	process;
	
	struct {
		//Number (*read)(Byte* buffer, Number size_of_buffer);
		//Number (*write)(Byte* buffer, Number size_of_buffer);
		Number (*read_character)(Byte* source);
		void (*write_character)(Byte* source, Number character);
	}
	pipe;
	
	struct {
		struct {
			Number  (*get_number_of_modes) ();
			Boolean (*get_mode_info)       (Number mode_index, Pixel_Mode* mode);
			Boolean (*set_mode)            (Number mode_index);
		}
		pixel;
		
		struct {
			Number (*get_cursor_position_x) ();
			Number (*get_cursor_position_y) ();
			Number (*get_text_color)        ();
			Number (*get_background_color)  ();
			void   (*set_mode)              ();
			void   (*set_cursor_position)   (Number x, Number y);
			void   (*set_text_color)        (Number color);
			void   (*set_background_color)  (Number color);
			void   (*write_character)       (Byte* display, Number character);
		}
		text;
	}
	display;
	
	struct {
		Number (*read_character)       (Byte* keyboard);
		void   (*set_key_down_handler) (void(*key_down_handler)(Byte key_code, Boolean is_special));
		void   (*set_key_up_handler)   (void(*key_up_handler)(Byte key_code, Boolean is_special));
	}
	keyboard;
	
	void (*reset) ();
}
API;


typedef void(*Process_Start)(API* api);


typedef struct {
	struct Process* next;
	struct Process* previous_piping_process;
	struct Process* next_piping_process;
	
	struct Process* calling_process;
	
	API*          api;
	Process_Start start;
	Boolean       started;
	
	Number esp;
	Number ebp;
	
	Number exit_esp;
	Number exit_ebp;
	
	Boolean previous_piping_process_stopped;
	Boolean next_piping_process_stopped;
	Number  previous_character;
	Boolean has_character;
}
Process;


#define CURRENT_PROCESS_POINTER_ADDRESS (65536)
#define get_module_address() \
	Number module_address = (*(*((Process***)CURRENT_PROCESS_POINTER_ADDRESS)))->start;


#endif//API_INCLUDED