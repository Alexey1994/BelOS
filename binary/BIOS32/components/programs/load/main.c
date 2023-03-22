#include <API.c>


API* _api;


Number main(Number number_of_arguments, Byte** arguments);


void start(API* api)
{
	get_module_address();

	*(API**)((Byte*)&_api + module_address) = api;
	main(api->process.number_of_arguments, api->process.arguments);
}


#include <memory.c>
#include <writer.c>


void print(Byte* parameters, ...)
{
	get_module_address();
	API* api = *(API**)((Byte*)&_api + module_address);
	
	print_in_source(0, api->pipe.write_character, parameters, &parameters + 1);
}


void print_error(Byte* parameters, ...)
{
	get_module_address();
	API* api = *(API**)((Byte*)&_api + module_address);
	
	print_in_source(0, api->display.text.write_character, parameters, &parameters + 1);
}


void print_directory(API* api)
{
	get_module_address();
	
	File_Enumerator enumerator;
	Boolean         is_many_files;

	is_many_files = 0;
	
	enumerator.data_cluster   = 0;
	enumerator.cluster_offset = 0;
	enumerator.file_number    = 0;
	
	while(api->file.enumerate(&enumerator)) {
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
	
	FAT_Data file;
	
	if(!api->file.open(&file, file_name)) {
		print_error("file %s not found" + module_address, file_name);
		return;
	}
	
	Byte   sector1[512];
	Byte   sector2[512];
	Byte*  last_sector;
	Byte*  previous_sector;
	Number i;

	/*
	while(api->file.read_sector(&file, sector)) {
		for(i = 0; i < 512; ++i) {
			api->console.print("%c", sector[i]);
		}
	}*/
	
	last_sector = sector1;
	
	if(api->file.read_sector(&file, last_sector)) {
		for(;;) {
			previous_sector = last_sector;
			
			if(last_sector == sector1) {
				last_sector = sector2;
			}
			else {
				last_sector = sector1;
			}
			
			if(!api->file.read_sector(&file, last_sector)) {
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
	
	print("%s addr=%d ESP=%d EBP=%d" + module_address, process->api->process.arguments[0], process, process->esp, process->ebp);
}


Number main(Number number_of_arguments, Byte** arguments)
{
	get_module_address();
	API* api = *(API**)((Byte*)&_api + module_address);
	
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
		
		first_process = api->process.get_first();
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