#include <types.c>


void (*set_interrupt_handler)(
	void(*handler)(),
	Number32 interrupt_number
);

void (*clean_interrupt_handler)(Number32 interrupt_number);

Boolean (*read_sector)(
	Number32 start_sector,
	Byte*  sector
);

void (*reset)();


void start_shell();


void _start(
	void (*_set_interrupt_handler)(
		void(*handler)(),
		Number32 interrupt_number
	),
	
	void (*_clean_interrupt_handler)(Number32 interrupt_number),
	
	Boolean (*_read_sector)(
		Number32 start_sector,
		Byte*  sector
	),
	
	void (*_reset)()
)
{
	set_interrupt_handler = _set_interrupt_handler;
	clean_interrupt_handler = _clean_interrupt_handler;
	read_sector = _read_sector;
	reset = _reset;
	
	start_shell();
}


#include "interfaces/IO.c"
#include "devices/timer.c"
#include "devices/keyboard.c"
#include "devices/console.c"
#include "FAT/read.c"
#include <API.c>

#include "shell/init.c"