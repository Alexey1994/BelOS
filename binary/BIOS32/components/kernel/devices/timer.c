#ifndef TIMER_INCLUDED
#define TIMER_INCLUDED


#define NTSC_FREQUENCY 3579545
//#define BASE_FREQUENCY NTSC_FREQUENCY * 4
//#define PIT_FREQUENCY NTSC_FREQUENCY / 3


Number number_of_ticks = 0;
Number frequency_divider = 65535;


void set_timer_frequency_divider(Number timer_number, Number divider)
{
	if(timer_number > 2) {
		return;
	}
	
	asm("cli");
	out_8(0x43, (timer_number << 6) | 0b00110100); //channel_number, lobyte/hibyte, rate generator
	out_8(0x40, divider & 0xFF);
	out_8(0x40, (divider >> 8) & 0xFF);
	frequency_divider = divider;
	asm("sti");
}


asm("pusha");
asm("call interrupt_32_handler");
asm("mov $0x20, %al");
asm("out %al, $0x20");
asm("popa");
asm("iret");
void interrupt_32_handler()
{
	++number_of_ticks;
}


void sleep(Number time)
{
	Number end_time;
	
	//end_time = number_of_ticks + (time * 18) / 1000;
	end_time = number_of_ticks + time * NTSC_FREQUENCY / 1000 / 3 / frequency_divider;

	do {
		asm("hlt");
	}
	while(number_of_ticks < end_time);
}


#endif//TIMER_INCLUDED