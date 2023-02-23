#include <API.c>


void main(API* api)
{
	Number character;
	
	for(;;) {
		//api->write_character(0, api->read_character(0));
		
		character = api->read_character(0);
		api->write_character(0, character);
		api->write_character(0, character);
	}
}