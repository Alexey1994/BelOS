#include <API.c>


API* _api;


Number main(Number number_of_arguments, Byte** arguments);


void start(API* api)
{
	get_module_address();
	
	*(API**)((Byte*)&_api + module_address) = api;
	main(api->process.number_of_arguments, api->process.arguments);
}


#include <writer.c>


void print(Byte* parameters, ...)
{
	get_module_address();
	API* api = *(API**)((Byte*)&_api + module_address);
	
	print_in_source(0, api->pipe.write_character, parameters, &parameters + 1);
}


#define MAX_NUMBER_OF_STORED_COMMANDS 5


typedef struct {
	Byte*    initial_heap_value;
	Process* process;
	
	Byte   command[256];
	Number command_size;
	
	Number cursor_pos_x;
	Number cursor_pos_y;
	
	Number number_of_stored_commands;
	Number current_stored_command;
	Byte   stored_commands[MAX_NUMBER_OF_STORED_COMMANDS * 64];
}
Shell;


Shell _shell;


#include "input.c"


void store_command(Byte* command)
{
	get_module_address();
	Shell* shell = (Shell*)((Byte*)&_shell + module_address);
	
	
	Number i;
	Number j;
	
	if(!compare_null_terminated_bytes(command, shell->stored_commands + (shell->number_of_stored_commands - 1) * 64)) {
		return;
	}
	
	if(shell->number_of_stored_commands < MAX_NUMBER_OF_STORED_COMMANDS) {
		++shell->number_of_stored_commands;
	}
	
	if(shell->number_of_stored_commands == MAX_NUMBER_OF_STORED_COMMANDS) {
		for(i = 0; i < shell->number_of_stored_commands - 1; ++i) {
			for(j = 0; j < 64; ++j) {
				shell->stored_commands[i * 64 + j] = shell->stored_commands[(i + 1) * 64 + j];
			}
		}
	}
	
	for(j = 0; j < 64; ++j) {
		shell->stored_commands[(shell->number_of_stored_commands - 1) * 64 + j] = command[j];
	}
	
	shell->current_stored_command = shell->number_of_stored_commands;
}


Number main(Number number_of_arguments, Byte** arguments)
{
	get_module_address();
	API* api = *(API**)((Byte*)&_api + module_address);
	Shell* shell = (Shell*)((Byte*)&_shell + module_address);
	
	
	api->display.text.set_mode();
	
	shell->process = api->process.get_first();
	while(shell->process->api != api) {
		shell->process = shell->process->next;
	}
	
	shell->initial_heap_value = api->heap.allocate(0);
	shell->command[0] = '\0';
	shell->command_size = 0;
	shell->number_of_stored_commands = 0;
	shell->current_stored_command = 0;
	
	
	api->display.text.set_text_color(10);
	print(">" + module_address);
	api->display.text.set_text_color(15);
	
	shell->cursor_pos_x = api->display.text.get_cursor_position_x();
	shell->cursor_pos_y = api->display.text.get_cursor_position_y();
	
	api->keyboard.set_key_down_handler(&on_key_down + module_address);
	
	for(;;) {
		Number character;
		
		character = api->pipe.read_character(0);
		//api->pipe.write_character(0, character);
		
		
		if(character == '\n') {
			shell->cursor_pos_x = 1;
			
			shell->command[shell->command_size] = '\0';
			shell->command_size = 0;
			
			print("\n" + module_address);
			
			if(shell->command[0]) {
				/*on_key_down_in_program_handler = 0;
				on_key_up_in_program_handler = 0;
				on_key_input_handler = 0;
				on_key_down_handler = &on_break_key_down;
				on_key_up_handler = &on_break_key_up;
				screen_text_color = 7;
				execute_command(command);
				screen_text_color = 15;
				screen_background_color = 0;
				on_key_up_handler = 0;
				on_key_input_handler = &on_key_input;
				on_key_down_handler = &on_key_down;
				on_key_down_in_program_handler = 0;
				on_key_up_in_program_handler = 0;*/
				
				//print("%s" + module_address, shell->command);
				
				api->display.text.set_text_color(7);
				
				shell->process->next = 0;
				api->heap.free(shell->initial_heap_value);
				store_command(shell->command);
				api->keyboard.set_key_down_handler(0);
				api->process.execute(shell->command);
				
				api->display.text.set_mode();
				api->display.text.set_text_color(15);
				api->keyboard.set_key_down_handler(&on_key_down + module_address);
				
				print("\n" + module_address);
			}
			
			api->display.text.set_text_color(10);
			print(">" + module_address);
			api->display.text.set_text_color(15);
		}
		else if(character == '\t') {
			get_prompt();
		}
		else {
			put_command_char(character);
		}
	}
	
	return 0;
}
