void on_key_input(Byte char_code);
void on_key_down(Byte key_code, Boolean is_special);


void on_key_input(Byte char_code)
{
	if(char_code == '\n') {
		command[command_size] = '\0';
		command_size = 0;
		
		print("\n");
		
		if(command[0]) {
			on_key_input_handler = 0;
			on_key_down_handler = 0;
			//loader_api->clean_interrupt_handler(33);
			console_text_color = 7;
			execute_command(command);
			console_text_color = 15;
			console_background_color = 0;
			//loader_api->set_interrupt_handler((Number)&interrupt_33_handler - 12, 33);
			on_key_input_handler = &on_key_input;
			on_key_down_handler = &on_key_down;
			
			print("\n");
		}
		
		console_text_color = 10;
		print(">");
		console_text_color = 15;
	}
	else if(char_code == '\t') {
		
	}
	else {
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
		}
	}
}