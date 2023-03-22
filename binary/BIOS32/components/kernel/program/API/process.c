Process* first_process = 0;
Process* current_process = 0;


void switch_to_process(Process* process)
{
	if(process == current_process) {
		return;
	}

	//save exit context
	if(!process->started) {
		process->calling_process = current_process;
		asm("mov %%esp, %0":"=a"(process->exit_esp));
		asm("mov %%ebp, %0":"=a"(process->exit_ebp));
	}
	
	//save current process context
	if(current_process) {
		asm("mov %%esp, %0":"=a"(current_process->esp));
		asm("mov %%ebp, %0":"=a"(current_process->ebp));
	}
	
	//switch to process
	current_process = process;
	
	asm("mov %0, %%esp"::"a"(current_process->esp));
	asm("mov %0, %%ebp"::"a"(current_process->ebp));
	
	if(!current_process->started) {
		//print("[start %s %d]\n", current_process->api->process.arguments[0], current_process);
		
		current_process->started = 1;
		current_process->start(current_process->api);
		
		asm("exit_address:");
		{
			//print("[stop %s %d]\n", current_process->api->process.arguments[0], current_process);
			
			//free process
			Process** process_pointer;
				
			process_pointer = &first_process;
			
			while(*process_pointer != current_process) {
				process_pointer = &(*process_pointer)->next;
			}
			
			*process_pointer = current_process->next;
			
			
			Process* previous_piping_process;
			
			previous_piping_process = current_process->previous_piping_process;
			
			if(previous_piping_process) {
				previous_piping_process->next_piping_process_stopped = 1;
				//previous_piping_process->next_piping_process = 0;//current_process->next_piping_process;
			}
			
			Process* next_piping_process;
			
			next_piping_process = current_process->next_piping_process;
			
			if(next_piping_process) {
				next_piping_process->previous_piping_process_stopped = 1;
				//next_piping_process->previous_piping_process = 0;//current_process->previous_piping_process;
			}
			
			asm("mov %0, %%esp"::"a"(current_process->exit_esp));
			asm("mov %0, %%ebp"::"a"(current_process->exit_ebp));

			current_process = current_process->calling_process;
			
			
			//reset PIC, for exit from interrupt
			asm("mov $0x20, %al");
			asm("out %al, $0xA0");
			asm("out %al, $0x20");
		}
	}
}


void exit(Number code)
{
	if(!current_process->started) {
		return;
	}
	
	asm("jmp exit_address");
}




#include "../COM.c"
#include "../EXE.c"
#include "../ELF.c"


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


void initialize_program_api(API* api);


Boolean load_process(Process* process, Byte* command)
{
	Byte     program_name[13];
	Number   program_name_size;
	Number   extension_index;
	FAT_Data file;

	program_name_size = parse_program_name(command, program_name);
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


Process* get_first_process()
{
	return first_process;
}


Process** get_last_process_pointer()
{
	Process** last_process_pointer;
	
	last_process_pointer = &first_process;
	
	while(*last_process_pointer) {
		last_process_pointer = &(*last_process_pointer)->next;
	}
	
	return last_process_pointer;
}


Process* create_process(Byte* command, Process* previous_piping_process)
{
	Process* process;
	
	process = allocate_memory(sizeof(Process));
	process->next = 0;
	process->started = 0;
	
	process->api = allocate_memory(sizeof(API));
	initialize_program_api(process->api);
	
	if(!load_process(process, command)) {
		free_memory(process);
		return 0;
	}
	
	*get_last_process_pointer() = process;
	
	if(previous_piping_process) {
		previous_piping_process->next_piping_process = process;
	}
	
	process->previous_piping_process = previous_piping_process;
	process->previous_piping_process_stopped = 0;
	process->next_piping_process = 0;
	process->next_piping_process_stopped = 0;
	process->has_character = 0;
	process->previous_character = '\0';
	
	Number i;
	for(i = 0; command[i]; ++i) {
		process->api->process.command[i] = command[i];
	}
	process->api->process.command[i] = '\0';
	
	return process;
}


Boolean execute_command(Byte* command)
{
	Process*  process;
	Process*  previous_piping_process;
	Number    arguments_size;
	
	previous_piping_process = 0;

	for(;;) {
		process = create_process(command, previous_piping_process);
		
		if(!process) {
			return 0;
		}
		
		previous_piping_process = process;
		
		command = process->api->process.command;
		
		arguments_size = parse_arguments(
			command,
			&process->api->process.arguments,
			&process->api->process.number_of_arguments
		);
		
		if(command[arguments_size] != '|') {
			break;
		}
		
		command += arguments_size + 1;
		
		while(*command == ' ') {
			++command;
		}
	}
	
	switch_to_process(process);
	
	return 1;
}