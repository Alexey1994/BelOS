#ifndef WRITER_INCLUDED
#define WRITER_INCLUDED


#include "types.c"


void write_null_terminated_bytes(
	Byte* source,
	void (*write_character)(Byte* source, Number character),
	Byte* bytes
)
{
	for(; *bytes; ++bytes) {
		write_character(source, *bytes);
	}
}


void write_Number(
	Byte* source,
	void (*write_character)(Byte* source, Number character),
	Number number
)
{
	Number next;
	Byte   byte;
	
	next = number / 10;
	
	if(next) {
		write_Number(source, write_character, next);
	}

	write_character(source, number % 10 + '0');
}


void write_Number_triplets(
	Byte* source,
	void (*write_character)(Byte* source, Number character),
	Number number,
	Number level
)
{
	Number next;
	
	next = number / 10;
	
	if(next) {
		write_Number_triplets(source, write_character, next, level + 1);
	}
	
	write_character(source, number % 10 + '0');
	
	if(level && !(level % 3)) {
		write_character(source, ' ');
	}
}


void write_Signed_Number(
	Byte* source,
	void (*write_character)(Byte* source, Number character),
	Signed_Number number
)
{
	if(number < 0) {
		write_character(source, '-');
		number = -number;
	}
	
	write_Number(source, write_character, number);
}


void write_hex_character(
	Byte* source,
	void (*write_character)(Byte* source, Number character),
	Byte character
)
{
	if(character < 10) {
		write_character(source, character + '0');
	}
	else {
		write_character(source, character - 10 + 'A');
	}
}


void write_hex_Byte(
	Byte* source,
	void (*write_character)(Byte* source, Number character),
	Byte byte
)
{
	if(byte < 16) {
		write_character(source, '0');
		write_hex_character(source, write_character, byte);
	}
	else {
		write_hex_character(source, write_character, byte >> 4);
		write_hex_character(source, write_character, byte & 0b00001111);
	}
}


void print_in_source(
	Byte* source,
	void (*write_character)(Byte* source, Number character),
	Byte* parameters,
	Byte** arguments
)
{
	Byte parameter;

	for(;;) {
		parameter = *parameters;
		
		if(!parameter) {
			break;
		}
		
		++parameters;

		if(parameter == '%') {
			parameter = *parameters;
			++parameters;

			switch(parameter) {
				case 'c':
					write_character(source, *arguments);
					++arguments;
					break;

				case 'x':
					write_hex_Byte(source, write_character, *arguments);
					++arguments;
					break;

				case 'u':
					write_Number(source, write_character, *arguments);
					++arguments;
					break;

				case 'd':
					write_Signed_Number(source, write_character, *arguments);
					++arguments;
					break;
				
				case 't':
					write_Number_triplets(source, write_character, *arguments, 0);
					++arguments;
					break;

				case 's':
					write_null_terminated_bytes(source, write_character, *arguments);
					++arguments;
					break;

				default:
					write_character(source, parameter);
			}
		}
		else {
			write_character(source, parameter);
		}
	}
}


void write_character_in_bytes(Byte** source, Number character)
{
	**source = character;
	++*source;
}


void print_in_string(Byte* s, Byte* parameters, ...)
{
	print_in_source(&s, &write_character_in_bytes, parameters, &parameters + 1);
	write_character_in_bytes(&s, '\0');
}


#endif//WRITER_INCLUDED