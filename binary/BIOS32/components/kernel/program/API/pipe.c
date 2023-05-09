/*
Number read_from_pipe(Byte* buffer, Number size_of_buffer)
{
	Number i;
	
	if(current_process == first_process) {
		for(i = 0; i < size_of_buffer; ++i) {
			buffer[i] = read_character_from_keyboard_with_wait(0);
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

/*
Number read_character_from_pipe(Byte* source)
{
	if(current_process == first_process) {
		return read_character_from_keyboard_with_wait(0);
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
}*/


Number read_character_from_pipe(Byte* source)
{
	//print("[read from %s %d]\n", current_process->api->process.arguments[0], current_process);
	//print("[read ");
	//print_process(current_process);
	//print("]\n");
	
	Process* previous_piping_process;
	
	previous_piping_process = current_process->previous_piping_process;
	
	//if(current_process->previous_piping_process_stopped) {
	//	print("[must exit from read]");
	//	//return 256;
	//}
	
	if(previous_piping_process) {
		//print("[read]\n");
		//print("[previous_piping_process from %d is %d]\n", current_process, previous_piping_process);
		
		//if(current_process->previous_piping_process_stopped) {
		//	print("[must exit 3]");
		//	//return 256;
		//}
		
		if(!current_process->has_character) {
			//print("[no chars]\n"); //for(;;);
			//print("[switch to prev piping]");
			//if(current_process->previous_piping_process_stopped) {
			//	print("[must exit]");
			//	for(;;);
			//	//exit(0);
			//	return 256;
			//}
			
			switch_to_process(previous_piping_process);
			//print("[ret to pipe read]\n");
			
			//if(!current_process) {
			//	print("[current_process zeroed!!!]");
			//	for(;;);
			//}
			
			if(!current_process->has_character) {
				//print("[no characters!!!, prev %d cur %d]\n", previous_piping_process, current_process);
				//print("[no characters!!!, prev ");
				//print_process(previous_piping_process);
				//print(" cur ");
				//print_process(current_process);
				//print("]\n");
				//for(;;);
				
				return 256;
			}
			
			//if(current_process->previous_piping_process_stopped) {
				//current_process->has_character = 0;
				//print("[must exit 2]");
				//for(;;);
				//exit(0);
			//	return 256;
			//}
		}
		
		current_process->has_character = 0;
		
		//print("[ret from read]\n");
		return current_process->previous_character;
	}
	else {
		if(current_process->previous_piping_process_stopped) {
			//print("[must exit]");
			//exit(0);
			//print("[ret from read, must exit]\n");
			return 256;
		}
		
		//print("[ret from read]\n");
		//return read_character_from_keyboard_with_wait(source);
		Keyboard_Interface* keyboard_interface;
		keyboard_interface = get_interface_data("keyboard");
		
		if(keyboard_interface) {
			return keyboard_interface->read_character(source);
		}
		else {
			for(;;) {
				asm("hlt");
			}
		}
	}
}


void write_character_in_pipe(Byte* source, Number character)
{
	//print("[write %c from %s %d]\n", character, current_process->api->process.arguments[0], current_process);
	//print("[write ");
	//print_process(current_process);
	//print("]\n");
	
	Process* next_piping_process;
	
	next_piping_process = current_process->next_piping_process;
	
	//if(current_process->next_piping_process_stopped) {
	//	print("[must exit from write]");
	//	//exit(0);
	//}
	
	if(next_piping_process) {
		//print("[next_piping_process from %d is %d]\n", current_process, next_piping_process);
		
		//print("[write]\n");
		
		//if(next_piping_process->has_character) {
		//	print("[write in filled pipe]\n");
		//}
		
		next_piping_process->previous_character = character;
		next_piping_process->has_character = 1;
		
		switch_to_process(next_piping_process);
		
		//print("[ret from write in %d]\n", current_process);
		
		//if(!current_process) {
		//	print("[current_process zeroed!!!]");
		//	for(;;);
		//}
		
		//if(current_process->next_piping_process_stopped) {
		//	print("[must exit from write]");
		//	//exit(0);
		//}
	}
	else {
		//if(current_process->next_piping_process_stopped) {
		//	//exit(0);
		//	return;
		//}
		
		write_character_in_text_display(0, character);
	}
}