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


Boolean load_program(Process* process, Byte program_name[13], Number program_name_size)
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
			if(!load_COM_program(process, &file)) {
				goto execution_error;
			}
			
			return 1;
		}
		else if(
			to_upper_case(program_name[extension_index]) == 'E' &&
			to_upper_case(program_name[extension_index + 1]) == 'X' &&
			to_upper_case(program_name[extension_index + 2]) == 'E'
		) {
			if(!load_EXE_program(process, &file)) {
				goto execution_error;
			}
			
			return 1;
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
		if(!load_COM_program(process, &file)) {
			goto execution_error;
		}
		
		return 1;
	}
	
	program_name[extension_index]     = 'e';
	program_name[extension_index + 1] = 'x';
	program_name[extension_index + 2] = 'e';
	
	if(open_FAT_file(&fs,  &file, program_name)) {
		if(!load_EXE_program(process, &file)) {
			goto execution_error;
		}
		
		return 1;
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
	
	execution_error: {
		print(
			"%s not executed.",
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
	
	reset_heap();
	first_process = 0;
	
	store_command(command);
	
	Process** next_process_pointer;
	Process*  new_process;
	
	next_process_pointer = &first_process;
	
	for(;;) {
		new_process = allocate_memory(sizeof(Process));
		*next_process_pointer = new_process;
		new_process->next = 0;
		new_process->started = 0;
		next_process_pointer = &new_process->next;
		
		
		new_process->api = allocate_memory(sizeof(API));
		initialize_program_api(new_process->api);

		program_name_size = parse_program_name(command, program_name);
		arguments_size = parse_arguments(
			command,
			&new_process->api->process.arguments,
			&new_process->api->process.number_of_arguments
		);
		
		
		if(!load_program(new_process, program_name, program_name_size)) {
			return 0;
		}
		
		
		if(command[arguments_size] != '|') {
			break;
		}
		
		command += arguments_size + 1;
		
		while(*command == ' ') {
			++command;
		}
	}
	
	execute_main_process(first_process);
	
	return 1;
}