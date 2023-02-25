typedef void(*Process_Start)(API* api);


typedef struct {
	struct Process* next;
	
	API*          api;
	Process_Start start;
	Boolean       started;
	
	Number esp;
	Number ebp;
	
	Number  previous_character;
	Boolean has_character;
}
Process;


Process* first_process = 0;
Process* current_process = 0;


void switch_to_process(Process* process)
{
	if(process == current_process) {
		return;
	}
	
	//save current process context
	asm("mov %%esp, %0":"=a"(current_process->esp));
	asm("mov %%ebp, %0":"=a"(current_process->ebp));
	
	current_process = process;
	
	asm("mov %0, %%esp"::"a"(current_process->esp));
	asm("mov %0, %%ebp"::"a"(current_process->ebp));
	
	if(!current_process->started) {
		current_process->started = 1;
		current_process->start(current_process->api);
	}
}


Number32 kernel_stack;
Number32 kernel_base;


void execute_main_process(Process* main_process)
{
	//reset keyboard
	read_character_from_keyboard();
	
	asm("mov %esp, kernel_stack");
	asm("mov %ebp, kernel_base");
	
	//switch_to_process(main_process);
	current_process = main_process;
	asm("mov %0, %%esp"::"a"(current_process->esp));
	asm("mov %0, %%ebp"::"a"(current_process->ebp));
	current_process->started = 1;
	current_process->start(current_process->api);
	
	asm("exit_address:");
	
	//restore stack
	asm("mov kernel_stack, %esp");
	asm("mov kernel_base, %ebp");

	set_text_mode();
}


void exit(Number code)
{
	//reset PIC, for exit from interrupt
	asm("mov $0x20, %al");
	asm("out %al, $0xA0");
	asm("out %al, $0x20");
	
	asm("jmp exit_address");
}