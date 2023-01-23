#ifndef IO_INCLUDED
#define IO_INCLUDED


Number8 in_8(Number16 port)
{
	Number8 data;

	asm volatile(
		"inb %1, %0"
		: "=a"(data)
		: "Nd"(port)
	);

	return data;
}


void out_8(Number16 port, Number8 data)
{
	asm volatile(
		"outb %0, %1"
		:
		: "a"(data), "Nd"(port)
	);
}


Number16 in_16(Number16 port)
{
	Number16 data;

	asm volatile(
		"inw %1, %0"
		: "=a"(data)
		: "Nd"(port)
	);

	return data;
}


void out_16(Number16 port, Number16 data)
{
	asm volatile(
		"outw %0, %1"
		:
		: "a"(data), "Nd"(port)
	);
}


Number32 in_32(Number16 port)
{
	Number32 data;

	asm volatile(
		"inl %1, %0"
		: "=a"(data)
		: "Nd"(port)
	);

	return data;
}


void out_32(Number16 port, Number32 data)
{
	asm volatile(
		"outl %0, %1"
		:
		: "a"(data), "Nd"(port)
	);
}


#endif//IO_INCLUDED