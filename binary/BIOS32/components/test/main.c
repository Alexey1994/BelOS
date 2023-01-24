#include <API.c>


import extern API* api;


void _start(API* api)
{
	api->print("Hi");
	
	Number16* screen = 0xB8000;
	screen[0] = 'H' + (15 << 8);
}