#include <API.c>


void main(API* api)
{
	Byte* argument;
	
	argument = api->command_line;
	
	while(*argument && *argument != ' ') {
		++argument;
	}
	
	while(*argument && *argument == ' ') {
		++argument;
	}
	
	if(!*argument) {
		api->print("use cat <file_name>");
		return;
	}
	
	
	FAT_Data file;
	
	if(!api->open_file(&file, argument)) {
		api->print("file %s not found", argument);
		return;
	}
	
	Byte   sector1[512];
	Byte   sector2[512];
	Byte*  last_sector;
	Byte*  previouse_sector;
	Number i;

	/*
	while(api->read_file_sector(&file, sector)) {
		for(i = 0; i < 512; ++i) {
			api->print("%c", sector[i]);
		}
	}*/
	
	last_sector = sector1;
	
	if(api->read_file_sector(&file, last_sector)) {
		for(;;) {
			previouse_sector = last_sector;
			
			if(last_sector == sector1) {
				last_sector = sector2;
			}
			else {
				last_sector = sector1;
			}
			
			if(!api->read_file_sector(&file, last_sector)) {
				break;
			}
			
			for(i = 0; i < 512; ++i) {
				api->print("%c", previouse_sector[i]);
			}
		}
	
		for(i = 0; i < file.file_size % 512; ++i) {
			api->print("%c", previouse_sector[i]);
		}
	}
}