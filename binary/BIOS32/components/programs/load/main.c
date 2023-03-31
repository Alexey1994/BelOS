#include <API.c>


API* _api;

Pipe_Interface* _pipe_interface;
File_Interface* _file_interface;
Process_Interface* _process_interface;
Text_Display_Interface* _text_display_interface;


Number main(Number number_of_arguments, Byte** arguments);


void start(API* api)
{
	get_module_address();

	global(_api) = api;
	
	global(_pipe_interface) = api->get("pipe" + module_address);
	global(_file_interface) = api->get("file" + module_address);
	global(_process_interface) = api->get("process" + module_address);
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


void print_file(API* api, Byte* file_name)
{
	get_module_address();
	File_Interface* file_interface = global(_file_interface);
	
	
	FAT_Data file;
	
	if(!file_interface->open(&file, file_name)) {
		print_error("file %s not found" + module_address, file_name);
		return;
	}
	
	Byte   sector1[512];
	Byte   sector2[512];
	Byte*  last_sector;
	Byte*  previous_sector;
	Number i;

	/*
	while(file_interface->read_sector(&file, sector)) {
		for(i = 0; i < 512; ++i) {
			api->console.print("%c", sector[i]);
		}
	}*/
	
	last_sector = sector1;
	
	if(file_interface->read_sector(&file, last_sector)) {
		for(;;) {
			previous_sector = last_sector;
			
			if(last_sector == sector1) {
				last_sector = sector2;
			}
			else {
				last_sector = sector1;
			}
			
			if(!file_interface->read_sector(&file, last_sector)) {
				break;
			}
			
			for(i = 0; i < 512; ++i) {
				print("%c" + module_address, previous_sector[i]);
			}
		}
	
		for(i = 0; i < file.file_size % 512; ++i) {
			print("%c" + module_address, previous_sector[i]);
		}
	}
}


void print_process(Process* process)
{
	get_module_address();
	
	print("%s addr=%d ESP=%d EBP=%d" + module_address, process->api->arguments[0], process, process->esp, process->ebp);
}


Number main(Number number_of_arguments, Byte** arguments)
{
	get_module_address();
	API* api = global(_api);
	Process_Interface* process_interface = global(_process_interface);
	
	
	if(number_of_arguments < 2) {
		goto error;
	}
	
	if(!compare_null_terminated_bytes(arguments[1], "file" + module_address)) {
		if(number_of_arguments < 3) {
			goto error;
		}
	
		print_file(api, arguments[2]);
	}
	else if(!compare_null_terminated_bytes(arguments[1], "string" + module_address)) {
		if(number_of_arguments < 3) {
			goto error;
		}
		
		print("%s" + module_address, arguments[2]);
	}
	else if(!compare_null_terminated_bytes(arguments[1], "dir" + module_address)) {
		print_directory(api);
	}
	else if(!compare_null_terminated_bytes(arguments[1], "processes" + module_address)) {
		Process* first_process;
		Process* process;
		
		first_process = process_interface->get_first();
		process = first_process;
		
		while(process) {
			if(process != first_process) {
				print("\n" + module_address);
			}
			
			print_process(process);
			process = process->next;
		}
	}
	else {
		goto error;
	}
	
	return 0;
	
	error: {
		print_error(
			"usage:"
			"\n\tload file <file_name>"
			"\n\tload string <string>"
			"\n\tload dir"
			"\n\tload processes"
			+ module_address
		);
	}
	
	return 1;
}