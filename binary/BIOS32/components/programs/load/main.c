#include <API.c>


API* _api;


void print(Byte* parameters, ...);

void print_directory(API* api);
void print_file(API* api, Byte* file_name);
Signed_Number compare_null_terminated_bytes(Byte* bytes1, Byte* bytes2);


void main(API* api)
{
	get_module_address(main);

	*(API**)((Byte*)&_api + module_address) = api;
	
	if(api->process.number_of_arguments < 2) {
		goto error;
	}
	
	if(!compare_null_terminated_bytes(api->process.arguments[1], "string" + module_address)) {
		if(api->process.number_of_arguments < 3) {
			goto error;
		}
		
		print("%s" + module_address, api->process.arguments[2]);
	}
	else if(!compare_null_terminated_bytes(api->process.arguments[1], "dir" + module_address)) {
		print_directory(api);
	}
	else if(!compare_null_terminated_bytes(api->process.arguments[1], "file" + module_address)) {
		if(api->process.number_of_arguments < 3) {
			goto error;
		}
	
		print_file(api, api->process.arguments[2]);
	} 
	else {
		goto error;
	}
	
	return;
	
	error: {
		api->console.print(
			"usage:"
			"\n\tload string <string>"
			"\n\tload dir"
			"\n\tload file <file_name>"
			+ module_address
		);
	}
}


#include <writer.c>


void print(Byte* parameters, ...)
{
	get_module_address(print);
	API* api = *(API**)((Byte*)&_api + module_address);
	//api->console.print("k" + module_address);
	
	print_in_source(0, api->write_character, parameters, &parameters + 1);
}


void print_directory(API* api)
{
	get_module_address(print_directory);
	
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
	get_module_address(print_file);
	
	FAT_Data file;
	
	if(!api->file.open(&file, file_name)) {
		api->console.print("file %s not found" + module_address, file_name);
		return;
	}
	
	Byte   sector1[512];
	Byte   sector2[512];
	Byte*  last_sector;
	Byte*  previouse_sector;
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
			previouse_sector = last_sector;
			
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
				print("%c" + module_address, previouse_sector[i]);
			}
		}
	
		for(i = 0; i < file.file_size % 512; ++i) {
			print("%c" + module_address, previouse_sector[i]);
		}
	}
}


#include <memory.c>