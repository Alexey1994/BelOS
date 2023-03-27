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
#include "devices/text display.c"
#include "devices/keyboard.c"
#include "devices/mouse.c"

#include "program/init.c"


void start_shell()
{
	*((Process**)CURRENT_PROCESS_POINTER_ADDRESS) = &current_process;
	
	open_root();
	
	set_timer_frequency_divider(0, 1193);
	loader_api->set_interrupt_handler((Number)&interrupt_32_handler - 12, 32);
	
	loader_api->set_interrupt_handler((Number)&interrupt_33_handler - 12, 33);
	
	loader_api->set_interrupt_handler((Number)&interrupt_44_handler - 14, 44);
	initialize_ps2_mouse();
	
	
	Process* shell_process;

	shell_process = create_process("shell", 0);
	
	if(shell_process) {
		parse_arguments(
			shell_process->api->process.command,
			&shell_process->api->process.arguments,
			&shell_process->api->process.number_of_arguments
		);
		
		switch_to_process(shell_process);
	}
}