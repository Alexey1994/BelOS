#include <API.c>


void main(API* api)
{
	Number character;
	
	for(;;) {
		//api->pipe.write_character(0, api->pipe.read_character(0));
		
		character = api->pipe.read_character(0);
		api->pipe.write_character(0, character);
		api->pipe.write_character(0, character);
	}
}