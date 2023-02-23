typedef void(*Process_Start)(API* api);


Number32 kernel_stack;
Number32 kernel_base;
Number32 program_stack = 128 * 1024 * 1024;


void execute(Process_Start start, API* api)
{
	//reset keyboard
	read_character_from_keyboard();
	
	asm("mov %esp, kernel_stack");
	asm("mov %ebp, kernel_base");
	asm("mov program_stack, %esp");

	start(api);
	
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