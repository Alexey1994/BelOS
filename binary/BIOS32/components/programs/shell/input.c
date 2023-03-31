void put_command_char(Byte char_code)
{
	get_module_address();
	Shell* shell = global_ptr(_shell);
	Pipe_Interface* pipe_interface = global(_pipe_interface);
	
	
	Number i;
	
	if(shell->cursor_pos_x >= 80) {
		return;
	}
	
	for(i = shell->command_size + 1; i != shell->cursor_pos_x; --i) {
		shell->command[i - 1] = shell->command[i - 2];
			
		Number16* text_display;
		text_display = 0xB8000;
		text_display[shell->cursor_pos_y * 80 + i] = text_display[shell->cursor_pos_y * 80 + i - 1];
	}

	shell->command[shell->cursor_pos_x - 1] = char_code;
	++shell->command_size;
	
	//print("%c" + module_address, char_code);
	pipe_interface->write_character(0, char_code);

	++shell->cursor_pos_x;
}


#include <string.c>
#include <memory.c>

void get_prompt()
{
	get_module_address();
	Shell* shell = global_ptr(_shell);
	File_Interface* file_interface = global(_file_interface);
	
	
	Byte   prompt[256];
	Number prompt_start;
	Number prompt_size;
	Number i;
	Number j;
	
	prompt_start = shell->cursor_pos_x - 1;
	prompt_size = 0;
	
	while(prompt_start && shell->command[prompt_start - 1] != ' ') {
		--prompt_start;
		++prompt_size;
	}
	
	for(i = 0; i < prompt_size; ++i) {
		prompt[i] = to_upper_case(shell->command[i + prompt_start]);
	}
	
	prompt[i] = '\0';
	

	File_Enumerator enumerator;

	enumerator.data_cluster   = 0;
	enumerator.cluster_offset = 0;
	enumerator.file_number    = 0;
	
	while(file_interface->enumerate(&enumerator)) {
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
	get_module_address();
	API* api = global(_api);
	Shell* shell = global_ptr(_shell);
	Text_Display_Interface* text_display_interface = global(_text_display_interface);
	
	
	if(is_special) {
		switch(key_code) {
			case KEY_ARROW_LEFT: {
				if(shell->cursor_pos_x <= 1) {
					return;
				}
				
				--shell->cursor_pos_x;
				
				text_display_interface->set_cursor_position(shell->cursor_pos_x, shell->cursor_pos_y);
				
				break;
			}
			
			case KEY_ARROW_RIGHT: {
				if(shell->cursor_pos_x - 1 >= shell->command_size) {
					return;
				}
				
				++shell->cursor_pos_x;
				
				text_display_interface->set_cursor_position(shell->cursor_pos_x, shell->cursor_pos_y);

				break;
			}
			
			case KEY_ARROW_UP: {
				if(!shell->number_of_stored_commands) {
					break;	
				}
				
				if(shell->current_stored_command) {
					--shell->current_stored_command;
				}
				
				shell->cursor_pos_x = 1;
				text_display_interface->set_cursor_position(shell->cursor_pos_x, shell->cursor_pos_y);
				shell->command_size = 0;
				
				Number i;
				
				//clear line
				for(i = 1; i < 79; ++i) {
					text_display_interface->write_character(0, ' ');
				}
				
				shell->cursor_pos_x = 1;
				text_display_interface->set_cursor_position(shell->cursor_pos_x, shell->cursor_pos_y);
				
				for(i = 0; i < 64 && shell->stored_commands[shell->current_stored_command * 64 + i]; ++i) {
					put_command_char(shell->stored_commands[shell->current_stored_command * 64 + i]);
				}

				break;
			}
			
			case KEY_ARROW_DOWN: {
				if(!shell->number_of_stored_commands) {
					break;	
				}
				
				++shell->current_stored_command;
				
				if(shell->current_stored_command >= shell->number_of_stored_commands) {
					shell->current_stored_command = shell->number_of_stored_commands - 1;
				}
				
				shell->cursor_pos_x = 1;
				text_display_interface->set_cursor_position(shell->cursor_pos_x, shell->cursor_pos_y);
				shell->command_size = 0;
				
				Number i;
				
				//clear line
				for(i = 1; i < 79; ++i) {
					text_display_interface->write_character(0, ' ');
				}
				
				shell->cursor_pos_x = 1;
				text_display_interface->set_cursor_position(shell->cursor_pos_x, shell->cursor_pos_y);
				
				for(i = 0; i < 64 && shell->stored_commands[shell->current_stored_command * 64 + i]; ++i) {
					put_command_char(shell->stored_commands[shell->current_stored_command * 64 + i]);
				}
				
				break;
			}
		}
	}
	else {
		switch(key_code) {
			case KEY_BACKSPACE: {
				if(shell->cursor_pos_x <= 1) {
					return;
				}
				
				--shell->cursor_pos_x;
				
				Number i;
				
				for(i = shell->cursor_pos_x; i < shell->command_size + 1; ++i) {
					shell->command[i - 1] = shell->command[i + 1 - 1];
					
					Number16* text_display;
					text_display = 0xB8000;
					text_display[shell->cursor_pos_y * 80 + i] = text_display[shell->cursor_pos_y * 80 + i + 1];
				}

				text_display_interface->set_cursor_position(shell->cursor_pos_x, shell->cursor_pos_y);
				
				--shell->command_size;
				
				break;
			}
			
			/*case KEY_TAB: {
				get_prompt();
				break;
			}*/
		}
	}
}