#include <API.c>


API* _api;


void print(Byte* parameters, ...);


void start(API* api)
{
	get_module_address();
	*(API**)((Byte*)&_api + module_address) = api;
	
	Number number_of_video_modes;
	Number i;
	
	number_of_video_modes = api->display.pixel.get_number_of_modes();
	print("%d modes:\n" + module_address, number_of_video_modes);
	
	for(i = 0; i < number_of_video_modes; ++i) {
		Pixel_Mode mode;
		
		api->display.pixel.get_mode_info(i, &mode);
		
		print(
			"%dx%dx%d, " + module_address,
			mode.width,
			mode.height,
			mode.bits_per_pixel
		);
	}
}


#include <writer.c>


void print(Byte* parameters, ...)
{
	get_module_address();
	API* api = *(API**)((Byte*)&_api + module_address);
	
	print_in_source(0, api->pipe.write_character, parameters, &parameters + 1);
}