#include <API.c>


void main(API* api)
{
	api->print("Hi");
	
	Number16* screen = 0xB8000;
	screen[0] = 'H' + (15 << 8);
}