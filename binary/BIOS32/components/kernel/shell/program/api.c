#include <API.c>


void parse_program_arguments(API* api, Byte* command_line)
{
	api->number_of_arguments = 0;	

	while(*command_line && api->number_of_arguments < sizeof(api->arguments) / sizeof(api->arguments[0])) {
		api->arguments[api->number_of_arguments] = command_line;
		++api->number_of_arguments;
		
		while(*command_line && *command_line != ' ') {
			++command_line;
		}
		
		if(*command_line == ' ') {
			*command_line = '\0';
			++command_line;
		}
		
		while(*command_line && *command_line == ' ') {
			++command_line;
		}
	}
}


void initialize_program_api(API* api)
{
	api->print            = &print;
	api->enum_files       = &enum_files;
	api->open_file        = &open_file;
	api->read_file_sector = &read_file_sector;
	api->reset            = loader_api->reset;
	
	parse_program_arguments(api, command);
}