#include <API.c>


void main(API* api)
{
	Number number_of_video_modes;
	Number i;
	
	number_of_video_modes = api->screen.get_number_of_video_modes();
	api->console.print("%d modes:\n", number_of_video_modes);
	
	for(i = 0; i < number_of_video_modes; ++i) {
		api->console.print(
			"%dx%dx%d, ",
			api->screen.get_video_mode_width(i),
			api->screen.get_video_mode_height(i),
			api->screen.get_video_mode_bits_per_pixel(i)
		);
	}
}