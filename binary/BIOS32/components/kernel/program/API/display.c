typedef struct {
	Byte     signature[4]; //VESA
	Number16 version;
	Number32 oem_string;
	Number32 capabilities;
	Number32 video_modes; //Number16*
	Number16 total_memory;
}
VESA_Info;


typedef struct {
	Number16 attributes;
	Number8  window_a;
	Number8  window_b;
	Number16 granularity;
	Number16 window_size;
	Number16 segment_a;
	Number16 segment_b;
	Number32 win_func_ptr;
	Number16 pitch;
	Number16 width;
	Number16 height;
	Number8  w_char;
	Number8  y_char;
	Number8  planes;
	Number8  bpp;
	Number8  banks;
	Number8  memory_model;
	Number8  bank_size;
	Number8  image_pages;
	Number8  reserved0;
 
	Number8  red_mask;
	Number8  red_position;
	Number8  green_mask;
	Number8  green_position;
	Number8  blue_mask;
	Number8  blue_position;
	Number8  reserved_mask;
	Number8  reserved_position;
	Number8  direct_color_attributes;
 
	Number32 framebuffer;
	Number32 off_screen_mem_off;
	Number16 off_screen_mem_size;
	Number8  reserved1[206];
}
VESA_Mode_Info;


Boolean is_text_display_mode = 1;


void set_text_display_mode()
{
	if(is_text_display_mode) {
		return;
	}
	
	loader_api->set_text_mode();
	is_text_display_mode = 1;
}


Number get_number_of_VESA_modes()
{
	VESA_Info* vesa_info;
	Number16*  video_modes;
	Number     i;
	
	vesa_info = loader_api->get_VESA_Info();
	
	if(!vesa_info) {
		return 0;
	}
	
	video_modes = vesa_info->video_modes;
	
	for(i = 0; video_modes[i] != 0xFFFF; ++i) {
		
	}
	
	return i;
}


Number16 get_VESA_mode(Number video_mode_index)
{
	VESA_Info* vesa_info;
	Number16*  video_modes;
	Number     i;
	
	vesa_info = loader_api->get_VESA_Info();
	
	if(!vesa_info) {
		return 0;
	}
	
	video_modes = vesa_info->video_modes;
	
	for(i = 0; i != video_mode_index; ++i) {
		if(video_modes[i] == 0xFFFF) {
			return 0;
		}
	}
	
	return video_modes[i];
}


Boolean set_VESA_mode(Number video_mode_index)
{
	Number16 video_mode;
	
	video_mode = get_VESA_mode(video_mode_index);
	
	if(!video_mode) {
		return 0;
	}
	
	loader_api->set_VESA_mode(video_mode | 0x4000);
	is_text_display_mode = 0;
	
	return 1;
}


Boolean get_VESA_mode_info(Number video_mode_index, VESA_Mode_Info* video_mode_info)
{
	Number16 video_mode;
	
	video_mode = get_VESA_mode(video_mode_index);
	
	if(!video_mode) {
		return 0;
	}
	
	loader_api->get_VESA_mode_info(video_mode_info, video_mode | 0x4000);
	
	return 1;
}



VESA_Mode_Info video_mode_info;

Boolean get_pixel_display_mode_info(Number mode_index, Pixel_Mode* mode_info)
{
	if(!get_VESA_mode_info(mode_index, &video_mode_info)) {
		return 0;
	}
	
	mode_info->width          = video_mode_info.width;
	mode_info->pitch          = video_mode_info.pitch;
	mode_info->height         = video_mode_info.height;
	mode_info->bits_per_pixel = video_mode_info.bpp;
	mode_info->framebuffer    = video_mode_info.framebuffer;
	
	return 1;
}