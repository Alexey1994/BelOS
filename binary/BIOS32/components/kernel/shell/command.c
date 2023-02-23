#define MAX_NUMBER_OF_STORED_COMMANDS 5


Byte   stored_commands[MAX_NUMBER_OF_STORED_COMMANDS * 64];
Number number_of_stored_commands = 0;
Number current_stored_command = 0;


void store_command(Byte* command)
{
	Number i;
	Number j;
	
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
}


Number parse_program_name(Byte* command, Byte program_name[13])
{
	Number i;
	
	for(
		i = 0;
		i < 12 &&
			command[i] &&
			command[i] != ' ' &&
			command[i] != '|';
		++i
	) {
		program_name[i] = command[i];
	}
	
	program_name[i] = '\0';
	
	return i;
}


Number get_program_name_extension_index(Byte program_name[13], Number program_name_size)
{
	Number extension_index;
	
	for(extension_index = program_name_size; extension_index; --extension_index) {
		if(program_name[extension_index - 1] == '.') {
			break;
		}
	}
	
	return extension_index;
}


Number parse_arguments(Byte* command, Byte** arguments, Number* number_of_arguments)
{
	Number arguments_size;
	
	arguments_size = 0;
	*number_of_arguments = 0;
	
	while(command[arguments_size] && *number_of_arguments < MAX_NUMBER_OF_ARGUMENTS && command[arguments_size] != '|') {
		arguments[*number_of_arguments] = command + arguments_size;
		++*number_of_arguments;
		
		while(command[arguments_size] && command[arguments_size] != ' ' && command[arguments_size] != '|') {
			++arguments_size;
		}
		
		if(command[arguments_size] == ' ') {
			command[arguments_size] = '\0';
			++arguments_size;
		}
		else if(command[arguments_size] == '|') {
			Number i;
			for(i = 255; i > arguments_size; --i) {
				command[i] = command[i - 1];
			}
			
			command[arguments_size] = '\0';
			++arguments_size;
		}
		
		while(command[arguments_size] && command[arguments_size] == ' ') {
			++arguments_size;
		}
	}
	
	return arguments_size;
}


Process_Start load_program(Byte program_name[13], Number program_name_size, API* api)
{
	FAT_Data file;
	Number   extension_index;

	extension_index = get_program_name_extension_index(program_name, program_name_size);
	
	if(extension_index) {
		if(!open_FAT_file(&fs,  &file, program_name)) {
			goto not_found_error;
		}
		
		if(
			to_upper_case(program_name[extension_index]) == 'C' &&
			to_upper_case(program_name[extension_index + 1]) == 'O' &&
			to_upper_case(program_name[extension_index + 2]) == 'M'
		) {
			return load_COM_program(&file, api);
		}
		else if(
			to_upper_case(program_name[extension_index]) == 'E' &&
			to_upper_case(program_name[extension_index + 1]) == 'X' &&
			to_upper_case(program_name[extension_index + 2]) == 'E'
		) {
			return load_EXE_program(&file, api);
		}
		else {
			goto not_found_error;
		}
	}
	
	if(program_name_size >= 8) {
		goto not_found_error;
	}
	
	program_name[program_name_size++] = '.';
	extension_index = program_name_size;
	program_name[program_name_size++] = 'c';
	program_name[program_name_size++] = 'o';
	program_name[program_name_size++] = 'm';
	program_name[program_name_size] = '\0';
	
	if(open_FAT_file(&fs,  &file, program_name)) {
		return load_COM_program(&file, api);
	}
	
	program_name[extension_index]     = 'e';
	program_name[extension_index + 1] = 'x';
	program_name[extension_index + 2] = 'e';
	
	if(open_FAT_file(&fs,  &file, program_name)) {
		return load_EXE_program(&file, api);
	}
	
	program_name[extension_index - 1] = '\0';
	goto not_found_error;
	
	not_found_error: {
		print(
			"%s not found.\n"
			"Enter \"load dir\" for show all commands",
			program_name
		);
		
		return 0;
	}
}


Boolean execute_command(Byte* command)
{
	Byte   program_name[13];
	Number program_name_size;
	Number arguments_size;
	API*   api;
	
	reset_heap();
	first_process = 0;
	
	store_command(command);
	
	
	for(;;) {
		Process** previouse_process;
		
		previouse_process = &first_process;
		while(*previouse_process) {
			previouse_process = &((*previouse_process)->next);
		}
		*previouse_process = allocate_memory(sizeof(Process));
		(*previouse_process)->next = 0;
		
		
		api = allocate_memory(sizeof(API));
		initialize_program_api(api);

		program_name_size = parse_program_name(command, program_name);
		arguments_size = parse_arguments(command, &api->process.arguments, &api->process.number_of_arguments);
		
		
		(*previouse_process)->api = api;
		(*previouse_process)->started = 0;
		(*previouse_process)->start = load_program(program_name, program_name_size, api);
		
		if(!(*previouse_process)->start) {
			goto execution_error;
		}
		
		(*previouse_process)->esp = (Byte*)(*previouse_process)->start + 256 * 1024;
		(*previouse_process)->ebp = (*previouse_process)->esp;
		
		
		if(command[arguments_size] != '|') {
			break;
		}
		
		command += arguments_size + 1;
		
		while(*command == ' ') {
			++command;
		}
	}
	
	current_process = first_process;
	
	current_process->started = 1;
	execute(current_process->start, current_process->api);
	
	return 1;
	
	execution_error: {
		print(
			"%s not executed.",
			program_name
		);
		
		return 0;
	}
}