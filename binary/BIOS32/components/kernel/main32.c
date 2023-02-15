#include <types.c>


typedef struct {
	void    (*set_interrupt_handler)   (void(*handler)(), Number32 interrupt_number);
	void    (*clean_interrupt_handler) (Number32 interrupt_number);
	Boolean (*read_sector)             (Number32 start_sector, Byte*  sector);
	void    (*reset)                   ();
	
	void*   (*get_VESA_Info)           ();
	void    (*get_VESA_mode_info)      (void* mode_info, Number16 mode);
	void    (*set_VESA_mode)           (Number16 mode);
	void    (*set_text_mode)           ();
}
Loader_Api;


Loader_Api* loader_api;


void start_shell();


void _start(Loader_Api api)
{
	loader_api = &api;
	start_shell();
}


#include "interfaces/IO.c"
#include "devices/timer.c"
#include "devices/VESA.c"
#include "devices/keyboard.c"
#include "devices/screen.c"

#include "program/init.c"
#include "shell/init.c"