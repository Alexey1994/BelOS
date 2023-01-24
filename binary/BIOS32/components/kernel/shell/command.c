Boolean execute_command(Byte* command)
{
	Number i;
	Byte   program_name[13];
	Number extension_index;
	
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
	
	if(extension_index) {
		if(
			to_upper_case(program_name[extension_index]) == 'C' &&
			to_upper_case(program_name[extension_index + 1]) == 'O' &&
			to_upper_case(program_name[extension_index + 2]) == 'M'
		) {
			if(execute_COM_program(program_name, command)) {
				return;
			}
		}
		else if(
			to_upper_case(program_name[extension_index]) == 'E' &&
			to_upper_case(program_name[extension_index + 1]) == 'X' &&
			to_upper_case(program_name[extension_index + 2]) == 'E'
		) {
			if(execute_EXE_program(program_name, command)) {
				return;
			}
		}
		
		print("%s not found", program_name);
		return;
	}
	
	if(i >= 8) {
		print("%s not found", program_name);
		return;
	}
	
	program_name[i++] = '.';
	extension_index = i;
	program_name[i++] = 'c';
	program_name[i++] = 'o';
	program_name[i++] = 'm';
	program_name[i] = '\0';
	
	if(execute_COM_program(program_name, command)) {
		return;
	}
	
	program_name[extension_index]     = 'e';
	program_name[extension_index + 1] = 'x';
	program_name[extension_index + 2] = 'e';
	
	if(execute_EXE_program(program_name, command)) {
		return;
	}
	
	program_name[extension_index - 1] = '\0';
	print("%s not found", program_name);
}