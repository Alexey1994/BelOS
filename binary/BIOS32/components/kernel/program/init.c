#include <API.c>
#include "API/heap.c"
#include "API/video.c"
#include "API/file.c"
#include "API/keyboard.c"
#include "API/process.c"
#include "API/console.c"


typedef struct {
	struct Process* next;
	
	API*          api;
	Process_Start start;
	Boolean       started;
	
	Number  previouse_character;
	Boolean has_character;
	
	Number esp;
	Number ebp;
}
Process;


Process* first_process = 0;
Process* current_process = 0;

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
		//current_process = current_process->previouse;
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
			//save current process context
			asm("mov %%esp, %0":"=a"(current_process->esp));
			asm("mov %%ebp, %0":"=a"(current_process->ebp));
			
			//find previouse process
			Process* previouse_process;
			previouse_process = first_process;
			
			while(previouse_process->next != current_process) {
				previouse_process = previouse_process->next;
			}
			
			//switch to previouse process
			current_process = previouse_process;
			//TODO: current_process = current_process->previouse;
			
			asm("mov %0, %%esp"::"a"(current_process->esp));
			asm("mov %0, %%ebp"::"a"(current_process->ebp));
			asm("jmp return_address_from_read");
			
			asm("return_address_from_write:");
		}
		
		current_process->has_character = 0;
		
		return current_process->previouse_character;
	}
}


void write_character_in_pipe(Byte* source, Number character)
{
	if(!current_process->next) {
		write_character_in_screen(0, character);
	}
	else {
		//save current process context
		asm("mov %%esp, %0":"=a"(current_process->esp));
		asm("mov %%ebp, %0":"=a"(current_process->ebp));
		
		//switch to next process
		current_process = current_process->next;
		current_process->previouse_character = character;
		current_process->has_character = 1;
		
		if(!current_process->started) {
			current_process->started = 1;
			
			asm("mov %0, %%esp"::"a"(current_process->esp));
			asm("mov %0, %%ebp"::"a"(current_process->ebp));
			current_process->start(current_process->api);
		}
		else {
			asm("mov %0, %%esp"::"a"(current_process->esp));
			asm("mov %0, %%ebp"::"a"(current_process->ebp));
			asm("jmp return_address_from_write");
		}
		
		asm("return_address_from_read:");
	}
}


void initialize_program_api(API* api)
{
	//api->read = &read_from_pipe;
	//api->write = &write_in_pipe;
	api->read_character = &read_character_from_pipe;
	api->write_character = &write_character_in_pipe;
	
	api->process.sleep = &sleep;
	api->process.exit  = &exit;
	
	api->file.enumerate   = &enum_files;
	api->file.open        = &open_file;
	api->file.read_sector = &read_file_sector;
	
	api->console.read_character  = &read_character_from_console;
	api->console.write_character = &write_character_in_screen;
	api->console.print           = &print;
	
	api->keyboard.set_key_down_handler = &set_key_down_handler;
	api->keyboard.set_key_up_handler   = &set_key_up_handler;
	
	api->screen.get_number_of_video_modes     = &get_number_of_video_modes;
	api->screen.get_video_mode_width          = &get_video_mode_width;
	api->screen.get_video_mode_pitch          = &get_video_mode_pitch;
	api->screen.get_video_mode_height         = &get_video_mode_height;
	api->screen.get_video_mode_bits_per_pixel = &get_video_mode_bits_per_pixel;
	api->screen.get_video_mode_framebuffer    = &get_video_mode_framebuffer;
	api->screen.set_video_mode                = &set_video_mode;
	api->screen.set_text_mode                 = &set_text_mode;
	
	api->reset = loader_api->reset;
}


#include "COM.c"
#include "EXE.c"
#include "ELF.c"