#include <API.c>


void main(API* api)
{
	Number number_of_video_modes;
	Number i;
	
	number_of_video_modes = api->get_number_of_video_modes();
	api->print("%d modes:\n", number_of_video_modes);
	
	for(i = 0; i < number_of_video_modes; ++i) {
		api->print(
			"%dx%dx%d, ",
			api->get_video_mode_width(i),
			api->get_video_mode_height(i),
			api->get_video_mode_bits_per_pixel(i)
		);
	}
}