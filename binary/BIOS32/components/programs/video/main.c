#include <API.c>


API* _api;

Pipe_Interface* _pipe_interface;
Pixel_Display_Interface* _pixel_display_interface;


Number main(Number number_of_arguments, Byte** arguments);


void start(API* api)
{
	get_module_address();
	
	global(_api) = api;
	
	global(_pipe_interface) = api->get("pipe" + module_address);
	global(_pixel_display_interface) = api->get("display/pixel" + module_address);
	
	main(api->number_of_arguments, api->arguments);
}


#include <writer.c>


void print(Byte* parameters, ...)
{
	get_module_address();
	Pipe_Interface* pipe_interface = global(_pipe_interface);
	
	print_in_source(0, pipe_interface->write_character, parameters, &parameters + 1);
}


Number main(Number number_of_arguments, Byte** arguments)
{
	get_module_address();
	Pixel_Display_Interface* pixel_display_interface = global(_pixel_display_interface);
	
	
	Number number_of_video_modes;
	Number i;
	
	number_of_video_modes = pixel_display_interface->get_number_of_modes();
	print("%d modes:\n" + module_address, number_of_video_modes);
	
	for(i = 0; i < number_of_video_modes; ++i) {
		Pixel_Mode mode;
		
		pixel_display_interface->get_mode_info(i, &mode);
		
		print(
			"%dx%dx%d, " + module_address,
			mode.width,
			mode.height,
			mode.bits_per_pixel
		);
	}
	
	return 0;
}