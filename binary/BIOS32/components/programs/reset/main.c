#include <API.c>


API* _api;


void print_error(Byte* parameters, ...);


void start(API* api)
{
	get_module_address();
	*(API**)((Byte*)&_api + module_address) = api;
	
	api->reset();
	print_error("computer should be restart" + module_address);
}


#include <writer.c>


void print_error(Byte* parameters, ...)
{
	get_module_address();
	API* api = *(API**)((Byte*)&_api + module_address);
	
	print_in_source(0, api->display.text.write_character, parameters, &parameters + 1);
}