#include <API.c>


API* _api;

Pipe_Interface* _pipe_interface;
File_Interface* _file_interface;
Text_Display_Interface* _text_display_interface;


Number main(Number number_of_arguments, Byte** arguments);


void start(API* api)
{
	get_module_address();
	
	global(_api) = api;
	
	global(_pipe_interface) = api->get("pipe" + module_address);
	global(_file_interface) = api->get("file" + module_address);
	global(_text_display_interface) = api->get("display/text" + module_address);
	
	main(api->number_of_arguments, api->arguments);
}


#include <memory.c>
#include <writer.c>


void print(Byte* parameters, ...)
{
	get_module_address();
	Pipe_Interface* pipe_interface = global(_pipe_interface);
	
	print_in_source(0, pipe_interface->write_character, parameters, &parameters + 1);
}


void print_error(Byte* parameters, ...)
{
	get_module_address();
	Text_Display_Interface* text_display_interface = global(_text_display_interface);
	
	print_in_source(0, text_display_interface->write_character, parameters, &parameters + 1);
}


void print_directory(API* api)
{
	get_module_address();
	File_Interface* file_interface = global(_file_interface);
	
	
	File_Enumerator enumerator;
	Boolean         is_many_files;

	is_many_files = 0;
	
	enumerator.data_cluster   = 0;
	enumerator.cluster_offset = 0;
	enumerator.file_number    = 0;
	
	while(file_interface->enumerate(&enumerator)) {
		if(is_many_files) {
			print("\n" + module_address);
		}
		
		print("%s" + module_address, enumerator.file_data->name);
		is_many_files = 1;
	}
}


Number main(Number number_of_arguments, Byte** arguments)
{
	get_module_address();
	API* api = global(_api);
	
	
	if(number_of_arguments < 2) {
		print_error(
			"usage:"
			"\n\topen <directory_name>"
			+ module_address
		);
		
		goto error;
	}
	
	
	File_Interface* file_interface = global(_file_interface);
	
	
	Process* current_process = get_current_process();
	
	if(!current_process->calling_process) {
		print_error("no parent process to open\n" + module_address);
		goto error;
	}
	
	if(!file_interface->open_fs_directory(((Process*)current_process->calling_process)->file_system, arguments[1])) {
		print_error("%s not opened\n" + module_address, arguments[1]);
		
		goto error;
	}
	
	
	return 0;
	
	error: {
		return 1;
	}
}