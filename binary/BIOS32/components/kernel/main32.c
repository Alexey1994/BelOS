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


#include <API.c>


#include "interfaces/IO.c"
#include "interfaces/PCI.c"
//#include "interfaces/USB.c"

#include "devices/timer.c"
#include "devices/VESA.c"
#include "devices/text display.c"
//#include "devices/keyboard.c"
//#include "devices/mouse.c"

//#include "interfaces/USB.c"

#include "program/init.c"


void start_shell_process(Byte* name)
{
	Process* shell_process;

	shell_process = create_process(name, 0);
	
	if(shell_process) {
		parse_arguments(
			shell_process->api->command,
			&shell_process->api->arguments,
			&shell_process->api->number_of_arguments
		);
		
		switch_to_process(shell_process);
	}
}


void load_drivers()
{
	open_root();
	open_directory("DRIVERS");
	
	File_Enumerator enumerator;
	
	enumerator.data_cluster   = 0;
	enumerator.cluster_offset = 0;
	enumerator.file_number    = 0;
	
	while(enum_files(&enumerator)) {
		Byte driver_name[13];
		
		FAT_name_to_normal_name(enumerator.file_data->name, driver_name);
		execute_command(driver_name);
	}
}


void start_shell()
{
	*((Process**)CURRENT_PROCESS_POINTER_ADDRESS) = &current_process;
	
	
	set_timer_frequency_divider(0, 1193);
	loader_api->set_interrupt_handler((Number)&interrupt_32_handler - 12, 32);
	
	
	initialize_interfaces();
	load_drivers();
	
	//print_interfaces(open_root_interface(), 0);

	
	/*for(;;) {
		print("\n");
		configure_USB_devices();
		sleep(1000);
	}*/
	
	//configure_USB_devices();
	
	
	//wait(TIMER_EVENT, 5000);
	
	open_root();
	start_shell_process("shell");
}