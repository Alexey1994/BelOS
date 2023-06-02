#ifndef API_INCLUDED
#define API_INCLUDED


#include "types.c"
#include "file.c"
#include "display.c"
#include "keyboard.c"
#include "mouse.c"


#define MAX_NUMBER_OF_ARGUMENTS 256


typedef enum {
	TIMER_EVENT = 1
}
Event;


typedef struct {
	Boolean           is_exist;
	struct Interface* next;
	Byte              name[32];
	Boolean           is_folder;
	void*             data;
}
Interface;


typedef struct {
	void* (*allocate)         (Number block_size);
	Byte* (*allocate_aligned) (Number size, Number align);
	void  (*free)             (void* block);
}
Heap_Interface;


typedef struct {
	Boolean (*enumerate)      (File_Enumerator* enumerator);
	Boolean (*open_directory) (Byte* name);
	Boolean (*open)           (FAT_Data* file, Byte* name);
	Number  (*read_sector)    (FAT_Data* file, Byte* sector);
	
	Boolean (*enumerate_fs)      (void* fs, File_Enumerator* enumerator);
	Boolean (*open_fs_directory) (void* fs, Byte* name);
	Boolean (*fs_open)           (void* fs, FAT_Data* file, Byte* name);
	Number  (*read_fs_sector)    (void* fs, FAT_Data* file, Byte* sector);
}
File_Interface;


typedef struct {
	struct Process* (*get_first) ();
	struct Process* (*create)    (Byte* command, struct Process* previous_piping_process);
	void            (*switch_to) (struct Process* process);
	Boolean         (*execute)   (Byte* command);
	void            (*sleep)     (Number milliseconds);
	void            (*exit)      (Number code);
}
Process_Interface;


typedef struct {
	//Number (*read)(Byte* buffer, Number size_of_buffer);
	//Number (*write)(Byte* buffer, Number size_of_buffer);
	Number (*read_character)(Byte* source);
	void (*write_character)(Byte* source, Number character);
}
Pipe_Interface;


typedef struct {
	void  (*set)   (void(*handler)(), Number32 interrupt_number);
	void  (*clean) (Number32 interrupt_number);
}
Interrupt_Interface;


typedef struct {
	Number  (*get_number_of_modes) ();
	Boolean (*get_mode_info)       (Number mode_index, Pixel_Mode* mode);
	Boolean (*set_mode)            (Number mode_index);
}
Pixel_Display_Interface;


typedef struct {
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
Text_Display_Interface;


typedef struct {
	Number (*read_character)       (Byte* keyboard);
	void   (*set_key_down_handler) (void(*key_down_handler)(Byte key_code, Boolean is_special));
	void   (*set_key_up_handler)   (void(*key_up_handler)(Byte key_code, Boolean is_special));
}
Keyboard_Interface;


typedef struct {
	void (*set_handler) (void (*mouse_handler)(Mouse_Event* event));
}
Mouse_Interface;


#include <PCI.c>

typedef struct {
	Byte     (*read_byte_from_PCI_config)   (Number16 address, Number8 offset);
	Number16 (*read_2bytes_from_PCI_config) (Number16 address, Number8 offset);
	Number32 (*read_4bytes_from_PCI_config) (Number16 address, Number8 offset);
	void     (*read_bytes_from_PCI_config)  (Number16 address, Number8 offset, Byte* bytes, Number8 size);
	void     (*write_byte_in_PCI_config)    (Number16 address, Number8 offset, Byte byte);
	void     (*write_2bytes_in_PCI_config)  (Number16 address, Number8 offset, Number16 bytes);
	void     (*write_4bytes_in_PCI_config)  (Number16 address, Number8 offset, Number32 bytes);
	void     (*write_bytes_in_PCI_config)   (Number16 address, Number8 offset, Byte* bytes, Number8 size);
	Boolean  (*read_PCI_config)             (Number16 address, PCI_Config* config);
	Number32 (*get_PCI_MMIO_address)        (PCI_Config* config, Number BAR_number);
	Number32 (*get_PCI_IO_address)          (PCI_Config* config, Number BAR_number);
}
PCI_Interface;


typedef struct {
	void* (*get)(Byte* path);
	void  (*create)(Byte* path, void* data);
	
	//maybe will use it in kernel modules
	//Interface* (*open_root_interface)     ();
	//Interface* (*open_interface)          (Interface* parent_interface, Byte* name);
	//Interface* (*create_interface)        (Interface* parent_interface, Byte* name, void* data);
	//Interface* (*create_interface_folder) (Interface* parent_interface, Byte* name);
	//void       (*delete_interface)        (Interface* parent_interface, Byte* name);
	
	Number number_of_arguments;
	Byte*  arguments[MAX_NUMBER_OF_ARGUMENTS];
	Byte   command[256];
	
	
	Number (*wait) (Number first_event, ...);
}
API;


typedef void(*Process_Start)(API* api);


typedef struct {
	struct Process* next;
	struct Process* previous_piping_process;
	struct Process* next_piping_process;
	
	struct Process* calling_process;
	
	void*         file_system;
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
	
	Boolean         waiting;
	struct Process* next_process_in_event_queue;
	Number*         events; //null terminated
	//Number          number_of_events;
	Number          last_event;
}
Process;


#define CURRENT_PROCESS_POINTER_ADDRESS (65536)

#define get_current_process() \
	(*(*((Process***)CURRENT_PROCESS_POINTER_ADDRESS)))

//!!!DEPRECATED not compatible with interrupts, use get_module_address_by_function
#define get_module_address() \
	Number module_address = (*(*((Process***)CURRENT_PROCESS_POINTER_ADDRESS)))->start;

#define get_module_address_by_function(function) \
	Number module_address; \
	asm("call . + 5\n" "pop %0" : "=a"(module_address)); \
	module_address -= (Number)&(function) + 15;

#define global(name) *(void**)((Byte*)&name + module_address)
#define global_ptr(name) ((Byte*)&name + module_address)


#endif//API_INCLUDED