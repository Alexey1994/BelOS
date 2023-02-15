Number32 kernel_stack;
Number32 kernel_base;
Number32 program_stack = 128 * 1024 * 1024;


void execute(void(*start)(API* api), API* api)
{
	//reset keyboard
	read_character_from_keyboard();
	
	asm("mov %esp, kernel_stack");
	asm("mov %ebp, kernel_base");
	//asm("mov %esp, program_stack");

	start(api);
	asm("return_address:");

	set_text_mode();
}


void exit(Number code)
{
	//reset PIC (interrupt)
	asm("mov $0x20, %al");
	asm("out %al, $0xA0");
	asm("out %al, $0x20");
	
	//restore stack
	asm("mov kernel_stack, %esp");
	asm("mov kernel_base, %ebp");
	
	asm("jmp return_address");
}