#include <types.c>


typedef struct {
	void    (*set_interrupt_handler)   (void(*handler)(), Number32 interrupt_number);
	void    (*clean_interrupt_handler) (Number32 interrupt_number);
	Boolean (*read_sector)             (Number32 start_sector, Byte*  sector);
	void    (*reset)                   ();
}
Loader_Api;


void start_shell(Loader_Api* api);


void _start(Loader_Api api)
{
	start_shell(&api);
}


#include "interfaces/IO.c"
#include "devices/timer.c"
#include "devices/keyboard.c"
#include "devices/console.c"

#include "shell/init.c"