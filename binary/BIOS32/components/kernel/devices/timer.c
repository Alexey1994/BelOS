#ifndef TIMER_INCLUDED
#define TIMER_INCLUDED


Number number_of_ticks = 0;


asm (
	"pusha \n"
	"call interrupt_32_handler\n"
	"popa \n"
	"mov $0x20, %al \n"
	"out %al, $0x20 \n"
	"iret"
);
void interrupt_32_handler()
{
	++number_of_ticks;
}


void sleep(Number time)
{
	Number end_time;
	
	end_time = number_of_ticks + (time * 18) / 1000;

	do {
		asm("hlt");
	}
	while(number_of_ticks < end_time);
}


#endif//TIMER_INCLUDED