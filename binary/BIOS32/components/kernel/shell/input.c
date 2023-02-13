void on_key_input(Byte char_code);
void on_key_down(Byte key_code, Boolean is_special);


Boolean control_key_pressed = 0;


void on_break_key_down(Byte key_code, Boolean is_special)
{
	if(key_code == KEY_LEFT_CONTROL) {
		control_key_pressed = 1;
	}
	else if(key_code == KEY_C) {
		if(control_key_pressed) {
			exit(1);
		}
	}
	
	if(on_key_down_in_program_handler) {
		on_key_down_in_program_handler(key_code, is_special);
	}
}


void on_break_key_up(Byte key_code, Boolean is_special)
{
	if(key_code == KEY_LEFT_CONTROL) {
		control_key_pressed = 0;
	}
	
	if(on_key_up_in_program_handler) {
		on_key_up_in_program_handler(key_code, is_special);
	}
}


void put_command_char(Byte char_code)
{
	Number i;
	
	for(i = command_size + 1; i != cursor_pos_x; --i) {
		command[i - 1] = command[i - 2];
			
		Number16* screen;
		screen = 0xB8000;
		screen[cursor_pos_y * 80 + i] = screen[cursor_pos_y * 80 + i - 1];
	}

	command[cursor_pos_x - 1] = char_code;
	++command_size;
		
	print("%c", char_code);
}


void on_key_input(Byte char_code)
{
	if(char_code == '\n') {
		command[command_size] = '\0';
		command_size = 0;
		
		print("\n");
		
		if(command[0]) {
			on_key_down_in_program_handler = 0;
			on_key_up_in_program_handler = 0;
			on_key_input_handler = 0;
			on_key_down_handler = &on_break_key_down;
			on_key_up_handler = &on_break_key_up;
			//loader_api->clean_interrupt_handler(33);
			console_text_color = 7;
			execute_command(command);
			console_text_color = 15;
			console_background_color = 0;
			//loader_api->set_interrupt_handler((Number)&interrupt_33_handler - 12, 33);
			on_key_up_handler = 0;
			on_key_input_handler = &on_key_input;
			on_key_down_handler = &on_key_down;
			on_key_down_in_program_handler = 0;
			on_key_up_in_program_handler = 0;
			
			print("\n");
		}
		
		console_text_color = 10;
		print(">");
		console_text_color = 15;
	}
	else if(char_code == '\t') {
		
	}
	else {
		put_command_char(char_code);
	}
}


void get_prompt()
{
	Byte   prompt[256];
	Number prompt_start;
	Number prompt_size;
	Number i;
	Number j;
	
	prompt_start = cursor_pos_x - 1;
	prompt_size = 0;
	
	while(prompt_start && command[prompt_start - 1] != ' ') {
		--prompt_start;
		++prompt_size;
	}
	
	for(i = 0; i < prompt_size; ++i) {
		prompt[i] = to_upper_case(command[i + prompt_start]);
	}
	
	prompt[i] = '\0';
	

	File_Enumerator enumerator;

	enumerator.data_cluster   = 0;
	enumerator.cluster_offset = 0;
	enumerator.file_number    = 0;
	
	while(enum_files(&enumerator)) {
		if(!compare_bytes(enumerator.file_data->name, prompt_size, prompt, prompt_size)) {
			Byte   promted_file_name[12];
			Number promted_file_name_size = 0;
			
			for(j = 0; j < 8 && enumerator.file_data->name[j] != ' '; ++j) {
				promted_file_name[promted_file_name_size] = enumerator.file_data->name[j];
				++promted_file_name_size;
			}
			
			if(enumerator.file_data->name[8] != ' ') {
				promted_file_name[promted_file_name_size] = '.';
				++promted_file_name_size;
				
				for(j = 8; j < 11 && enumerator.file_data->name[j] != ' '; ++j) {
					promted_file_name[promted_file_name_size] = enumerator.file_data->name[j];
					++promted_file_name_size;
				}
			}
			
			for(j = prompt_size; j < promted_file_name_size; ++j) {
				put_command_char(promted_file_name[j]);
			}
			
			break;
		}
	}
}


void on_key_down(Byte key_code, Boolean is_special)
{
	if(is_special) {
		switch(key_code) {
			case KEY_ARROW_LEFT: {
				if(cursor_pos_x <= 1) {
					return;
				}
				
				--cursor_pos_x;
				
				set_cursor_position(cursor_pos_x, cursor_pos_y);
				
				break;
			}
			
			case KEY_ARROW_RIGHT: {
				if(cursor_pos_x - 1 >= command_size) {
					return;
				}
				
				++cursor_pos_x;
				
				set_cursor_position(cursor_pos_x, cursor_pos_y);

				break;
			}
			
			case KEY_ARROW_UP: {
				if(!number_of_stored_commands) {
					break;	
				}
				
				if(current_stored_command) {
					--current_stored_command;
				}
				
				cursor_pos_x = 1;
				command_size = 0;
				
				Number i;
				
				//clear line
				for(i = 1; i < 79; ++i) {
					write_character_in_console(0, ' ');
				}
				
				cursor_pos_x = 1;
				set_cursor_position(cursor_pos_x, cursor_pos_y);
				
				for(i = 0; i < 64 && stored_commands[current_stored_command * 64 + i]; ++i) {
					put_command_char(stored_commands[current_stored_command * 64 + i]);
				}

				break;
			}
			
			case KEY_ARROW_DOWN: {
				if(!number_of_stored_commands) {
					break;	
				}
				
				++current_stored_command;
				
				if(current_stored_command >= number_of_stored_commands) {
					current_stored_command = number_of_stored_commands - 1;
				}
				
				cursor_pos_x = 1;
				command_size = 0;
				
				Number i;
				
				//clear line
				for(i = 1; i < 79; ++i) {
					write_character_in_console(0, ' ');
				}
				
				cursor_pos_x = 1;
				set_cursor_position(cursor_pos_x, cursor_pos_y);
				
				for(i = 0; i < 64 && stored_commands[current_stored_command * 64 + i]; ++i) {
					put_command_char(stored_commands[current_stored_command * 64 + i]);
				}
				
				break;
			}
		}
	}
	else {
		switch(key_code) {
			case KEY_BACKSPACE: {
				if(cursor_pos_x <= 1) {
					return;
				}
				
				--cursor_pos_x;
				
				Number i;
				
				for(i = cursor_pos_x; i < command_size + 1; ++i) {
					command[i - 1] = command[i + 1 - 1];
					
					Number16* screen;
					screen = 0xB8000;
					screen[cursor_pos_y * 80 + i] = screen[cursor_pos_y * 80 + i + 1];
				}

				set_cursor_position(cursor_pos_x, cursor_pos_y);
				
				--command_size;
				
				break;
			}
			
			case KEY_TAB: {
				get_prompt();
				break;
			}
		}
	}
}