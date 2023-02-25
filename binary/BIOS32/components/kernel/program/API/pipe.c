/*
Number read_from_pipe(Byte* buffer, Number size_of_buffer)
{
	Number i;
	
	if(current_process == first_process) {
		for(i = 0; i < size_of_buffer; ++i) {
			buffer[i] = read_character_from_console(0);
		}
		
		return size_of_buffer;
	}
	else {
		//current_process = current_process->previous;
	}
}


Number write_in_pipe(Byte* buffer, Number size_of_buffer)
{
	Number i;
	
	if(!current_process->next) {
		for(i = 0; i < size_of_buffer; ++i) {
			write_character_in_screen(0, buffer[i]);
		}
		
		return size_of_buffer;
	}
	else {
		
		current_process = current_process->next;
	}
}*/


Number read_character_from_pipe(Byte* source)
{
	if(current_process == first_process) {
		return read_character_from_console(0);
	}
	else {
		if(!current_process->has_character) {
			//find previous process
			//TODO: current_process = current_process->previous;
			Process* previous_process;
			previous_process = first_process;
			
			while(previous_process->next != current_process) {
				previous_process = previous_process->next;
			}
			
			switch_to_process(previous_process);
		}
		
		current_process->has_character = 0;
		
		return current_process->previous_character;
	}
}


void write_character_in_pipe(Byte* source, Number character)
{
	if(!current_process->next) {
		write_character_in_screen(0, character);
	}
	else {
		Process* next_process;
		next_process = current_process->next;
		next_process->previous_character = character;
		next_process->has_character = 1;
		
		switch_to_process(next_process);
	}
}