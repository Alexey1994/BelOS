Boolean execute_command(Byte* command)
{
	Number   i;
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