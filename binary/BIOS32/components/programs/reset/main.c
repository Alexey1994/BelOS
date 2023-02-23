#include <API.c>


void main(API* api)
{
	get_module_address(main);
	
	api->reset();
	api->console.print("computer should be restart" + module_address);
}