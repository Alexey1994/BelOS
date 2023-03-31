#include <API.c>
#include "API/heap.c"
#include "API/display.c"
#include "API/file.c"
#include "API/keyboard.c"
#include "API/process.c"
#include "API/pipe.c"


#define FIRST_INTERFACE_ADDRESS (65536 * 2)
#define MAX_NUMBER_OF_INTERFACES 512


Interface* root_interface;


Interface* allocate_interface()
{
	Interface* allocated_interface;
	Number     i;
	
	for(i = 0; i < MAX_NUMBER_OF_INTERFACES; ++i) {
		allocated_interface = (Interface*)FIRST_INTERFACE_ADDRESS + i;
		
		if(!allocated_interface->is_exist) {
			clean_bytes(allocated_interface, sizeof(Interface));
			allocated_interface->is_exist = 1;
			return allocated_interface;
		}
	}
	
	return 0;
}


void free_interface(Interface* interface)
{
	clean_bytes(interface, sizeof(Interface));
}


Interface* open_root_interface()
{
	return root_interface;
}


Interface* open_interface(Interface* parent_interface, Byte* name)
{
	Interface* interface;
	
	if(!parent_interface) {
		parent_interface = open_root_interface();
	}
	
	interface = parent_interface->data;
	
	while(interface) {
		if(!compare_null_terminated_bytes(interface->name, name)) {
			return interface;
		}
		
		interface = interface->next;
	}
	
	return 0;
}


Interface* create_interface(Interface* parent_interface, Byte* name, void* data)
{
	Interface** interface_pointer;
	
	if(!parent_interface) {
		parent_interface = open_root_interface();
	}
	
	interface_pointer = &parent_interface->data;
	
	while(*interface_pointer) {
		interface_pointer = &(*interface_pointer)->next;
	}
	
	*interface_pointer = allocate_interface();
	copy_bytes((*interface_pointer)->name, name, 32);
	(*interface_pointer)->data = data;
	
	return *interface_pointer;
}


Interface* create_interface_folder(Interface* parent_interface, Byte* name)
{
	Interface** interface_pointer;
	
	if(!parent_interface) {
		parent_interface = open_root_interface();
	}
	
	interface_pointer = &parent_interface->data;
	
	while(*interface_pointer) {
		interface_pointer = &(*interface_pointer)->next;
	}
	
	*interface_pointer = allocate_interface();
	copy_bytes((*interface_pointer)->name, name, 32);
	(*interface_pointer)->is_folder = 1;
	
	return *interface_pointer;
}


void delete_interface(Interface* parent_interface, Byte* name)
{
	
}


void print_interfaces(Interface* root_interface, Number level)
{
	Interface* interface;
	Number     i;
	
	interface = root_interface->data;
	
	while(interface) {
		for(i = 0; i < level; ++i) {
			print("\t");
		}
		
		print("%s\n", interface->name);
		
		if(interface->is_folder) {
			print_interfaces(interface, level + 1);
		}
		
		interface = interface->next;
	}
}


Interface* get_interface(Byte* path)
{
	Interface* root;
	Byte       name[32];
	Number     i;
	
	root = open_root_interface();
	
	while(*path) {
		if(*path == '/') {
			++path;
		}

		for(i = 0; i < sizeof(name) - 1 && *path && *path != '/'; ++i) {
			name[i] = *path;
			++path;
		}
		name[i] = '\0';
		
		root = open_interface(root, name);
		
		if(!root) {
			return 0;
		}
	}
	
	return root;
}


void* get_interface_data(Byte* path)
{
	Interface* interface;
	
	interface = get_interface(path);
	
	if(interface) {
		return interface->data;
	}
	
	return 0;
}


Heap_Interface heap_interface = {
	.allocate = &allocate_memory,
	.free = &free_memory
};


File_Interface file_interface = {
	.enumerate   = &enum_files,
	.open        = &open_file,
	.read_sector = &read_file_sector
};


Process_Interface process_interface = {
	.get_first = &get_first_process,
	.create = &create_process,
	.switch_to = &switch_to_process,
	.execute = &execute_command,
	.sleep = &sleep,
	.exit  = &exit
};


Pipe_Interface pipe_interface = {
	//.read = &read_from_pipe,
	//.write = &write_in_pipe,
	.read_character = &read_character_from_pipe,
	.write_character = &write_character_in_pipe
};


Pixel_Display_Interface pixel_display_interface = {
	.get_number_of_modes = &get_number_of_VESA_modes,
	.get_mode_info       = &get_pixel_display_mode_info,
	.set_mode            = &set_VESA_mode
};


Text_Display_Interface text_display_interface = {
	.get_cursor_position_x    = &get_text_display_cursor_position_x,
	.get_cursor_position_y    = &get_text_display_cursor_position_y,
	.get_text_color           = &get_text_display_color,
	.get_background_color     = &get_text_display_background_color,
	.set_mode                 = &set_text_display_mode,
	.set_cursor_position      = &set_text_display_cursor_position,
	.set_text_color           = &set_text_display_color,
	.set_background_color     = &set_text_display_background_color,
	.write_character          = &write_character_in_text_display
};


Keyboard_Interface keyboard_interface = {
	.read_character       = &read_character_from_keyboard_with_wait,
	.set_key_down_handler = &set_key_down_handler,
	.set_key_up_handler   = &set_key_up_handler
};


Mouse_Interface mouse_interface = {
	
};


void initialize_interfaces()
{
	clean_bytes(FIRST_INTERFACE_ADDRESS, sizeof(Interface) * MAX_NUMBER_OF_INTERFACES);
	
	root_interface = allocate_interface();
	root_interface->is_folder = 1;
	
	
	create_interface(root_interface, "heap", &heap_interface);
	create_interface(root_interface, "file", &file_interface);
	create_interface(root_interface, "process", &process_interface);
	create_interface(root_interface, "pipe", &pipe_interface);
	
	Interface* display_interface;
	display_interface = create_interface_folder(root_interface, "display");
	create_interface(display_interface, "pixel", &pixel_display_interface);
	create_interface(display_interface, "text", &text_display_interface);
	
	create_interface(root_interface, "keyboard", &keyboard_interface);
	
	create_interface(root_interface, "reset", loader_api->reset);
	
	
	if(initialize_ps2_mouse()) {
		create_interface(root_interface, "mouse", &mouse_interface);
	}
	
	
	//print_interfaces(open_root_interface(), 0);
}


void initialize_program_api(API* api)
{
	api->get = &get_interface_data;
}