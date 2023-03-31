#include <API.c>


API* _api;

Text_Display_Interface* _text_display_interface;


Number main(Number number_of_arguments, Byte** arguments);


void start(API* api)
{
	get_module_address();
	
	global(_api) = api;
	
	global(_text_display_interface) = api->get("display/text" + module_address);
	
	main(api->number_of_arguments, api->arguments);
}


#include <writer.c>


void print_error(Byte* parameters, ...)
{
	get_module_address();
	Text_Display_Interface* text_display_interface = global(_text_display_interface);
	
	print_in_source(0, text_display_interface->write_character, parameters, &parameters + 1);
}


Number main(Number number_of_arguments, Byte** arguments)
{
	get_module_address();
	API* api = global(_api);
	
	
	((void(*)())api->get("reset" + module_address))();
	print_error("computer should be restart" + module_address);
	
	return 0;
}