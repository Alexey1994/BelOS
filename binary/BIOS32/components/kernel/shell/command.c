#define MAX_NUMBER_OF_STORED_COMMANDS 5


Byte   stored_commands[MAX_NUMBER_OF_STORED_COMMANDS * 64];
Number number_of_stored_commands = 0;
Number current_stored_command = 0;


Boolean execute_command(Byte* command)
{
	Number   i;
	Number   j;
	
	if(number_of_stored_commands < MAX_NUMBER_OF_STORED_COMMANDS) {
		++number_of_stored_commands;
	}
	
	if(number_of_stored_commands == MAX_NUMBER_OF_STORED_COMMANDS) {
		for(i = 0; i < number_of_stored_commands - 1; ++i) {
			for(j = 0; j < 64; ++j) {
				stored_commands[i * 64 + j] = stored_commands[(i + 1) * 64 + j];
			}
		}
	}
	
	for(j = 0; j < 64; ++j) {
		stored_commands[(number_of_stored_commands - 1) * 64 + j] = command[j];
	}
	
	current_stored_command = number_of_stored_commands;

	
	Byte     program_name[13];
	Number   extension_index;
	FAT_Data file;
	
	for(i = 0; i < sizeof(program_name) - 1 && command[i] && command[i] != ' '; ++i) {
		program_name[i] = command[i];
	}
	
	for(extension_index = i; extension_index; --extension_index) {
		if(program_name[extension_index] == '.') {
			++extension_index;
			break;
		}
	}
	
	program_name[i] = '\0';
	
	
	API api;

	initialize_program_api(&api);
	
	
	if(extension_index) {
		if(
			to_upper_case(program_name[extension_index]) == 'C' &&
			to_upper_case(program_name[extension_index + 1]) == 'O' &&
			to_upper_case(program_name[extension_index + 2]) == 'M'
		) {
			if(!open_FAT_file(&fs,  &file, program_name)) {
				goto not_found_error;
			}
			
			if(!execute_COM_program(&file, &api)) {
				goto execution_error;
			}
			
			return;
		}
		else if(
			to_upper_case(program_name[extension_index]) == 'E' &&
			to_upper_case(program_name[extension_index + 1]) == 'X' &&
			to_upper_case(program_name[extension_index + 2]) == 'E'
		) {
			if(!open_FAT_file(&fs,  &file, program_name)) {
				goto not_found_error;
			}
			
			if(!execute_EXE_program(&file, &api)) {
				goto execution_error;
			}
			
			return;
		}
		
		goto not_found_error;
	}
	
	if(i >= 8) {
		goto not_found_error;
	}
	
	program_name[i++] = '.';
	extension_index = i;
	program_name[i++] = 'c';
	program_name[i++] = 'o';
	program_name[i++] = 'm';
	program_name[i] = '\0';
	
	if(open_FAT_file(&fs,  &file, program_name)) {
		if(!execute_COM_program(&file, &api)) {
			goto execution_error;
		}
		
		return;
	}
	
	program_name[extension_index]     = 'e';
	program_name[extension_index + 1] = 'x';
	program_name[extension_index + 2] = 'e';
	
	if(open_FAT_file(&fs,  &file, program_name)) {
		if(!execute_EXE_program(&file, &api)) {
			goto execution_error;
		}
		
		return;
	}
	
	program_name[extension_index - 1] = '\0';
	goto not_found_error;
	
	not_found_error: {
		print(
			"%s not found."
			"\nEnter \"print dir\" for show all commands",
			program_name
		);
		
		return;
	}
	
	execution_error: {
		print(
			"%s not executed.",
			program_name
		);
		
		return;
	}
}