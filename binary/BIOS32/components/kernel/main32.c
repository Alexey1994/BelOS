#include <types.c>


void (*set_interrupt_handler)(
	void(*handler)(),
	Number32 interrupt_number
);

void (*clean_interrupt_handler)(Number32 interrupt_number);

Boolean (*read_sector)(
	Number32 start_sector,
	Byte*  sector
);

void (*reset)();


void main();


void _start(
	void (*_set_interrupt_handler)(
		void(*handler)(),
		Number32 interrupt_number
	),
	
	void (*_clean_interrupt_handler)(Number32 interrupt_number),
	
	Boolean (*_read_sector)(
		Number32 start_sector,
		Byte*  sector
	),
	
	void (*_reset)()
)
{
	set_interrupt_handler = _set_interrupt_handler;
	clean_interrupt_handler = _clean_interrupt_handler;
	read_sector = _read_sector;
	reset = _reset;
	
	main();
}


#include "interfaces/IO.c"
#include "devices/timer.c"
#include "devices/keyboard.c"
#include "devices/console.c"
#include "FAT/read.c"
#include <API.c>


FAT_File_System fs;

Byte command[256];
Number command_size = 0;

/*
void to_upper_case(Byte* source, Byte* destination)
{
	Byte c;
	
	for(;;) {
		c = *source;
		
		if(c >= 'a' && c <= 'z') {
			*destination = c + 'A' - 'a';
		}
		else {
			*destination = c;
		}
		
		if(!c) {
			break;
		}
		
		++source;
		++destination;
	}
}*/


Boolean enum_files(File_Enumerator* enumerator)
{
	if(!enumerator->data_cluster) {
		enumerator->data_cluster = fs.current_directory.cluster_number_low + (fs.current_directory.cluster_number_high << 16);
		enumerator->cluster_offset = 0;
		enumerator->file_number = 0;
	}
	
	return enum_FAT_data(&fs, enumerator);
}


Boolean open_file(FAT_Data* file, Byte* name)
{
	return open_FAT_file(&fs, file, name);
}


Number read_file_sector(FAT_Data* file, Byte* sector)
{
	return read_FAT_file_sector(&fs, file, sector);
}




Boolean execute_command();


void on_key_input(Byte char_code)
{
	if(char_code == '\n') {
		command[command_size] = '\0';
		command_size = 0;
		
		print("\n");
		
		if(command[0]) {
			execute_command();
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


Boolean execute_program(Byte* name, Byte* command_line)
{
	Byte*    program;
	void   (*program_start)(API* api);
	FAT_Data file;

	program_start = program = 1024 * 1024;
	
	if(!open_FAT_file(&fs,  &file, name)) {
		print("%s not found", name);
		return 0;
	}
	
	while(read_FAT_file_sector(&fs, &file, program)) {
		program += 512;
	}
	
	API api = {
		.command_line = command_line,
		.print = &print,
		.enum_files = &enum_files,
		.open_file = &open_file,
		.read_file_sector = &read_file_sector,
		.reset = reset,
	};

	on_key_input_handler = 0;
	on_key_down_handler = 0;
	//clean_interrupt_handler(33);
	program_start(&api);
	//set_interrupt_handler((Number)&interrupt_33_handler - 12, 33);
	on_key_input_handler = &on_key_input;
	on_key_down_handler = &on_key_down;
	
	
	/*
	Number i;
	
	program = 1024 * 1024;
	
	for(i = 0; i < file.file_size; ++i) {
		print("%c", program[i]);
	}*/
	
	return 1;
}


Boolean execute_command()
{
	Number i;
	Byte   program_name[11];
	
	for(i = 0; i < sizeof(program_name) && command[i] && command[i] != ' '; ++i) {
		program_name[i] = command[i];
	}
	
	program_name[i++] = '.';
	program_name[i++] = 'c';
	program_name[i++] = 'o';
	program_name[i++] = 'm';
		
	program_name[i] = '\0';
	
	execute_program(program_name, command);
}


void main()
{
	on_key_input_handler = &on_key_input;
	on_key_down_handler = &on_key_down;
	
	open_FAT_File_System(&fs, read_sector);

	set_interrupt_handler((Number)&interrupt_32_handler - 12, 32);
	set_interrupt_handler((Number)&interrupt_33_handler - 12, 33);

	console_text_color = 10;
	print(">");
	console_text_color = 15;
}