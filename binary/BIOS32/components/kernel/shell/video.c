Number get_number_of_video_modes()
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


Number16 get_video_mode(Number video_mode_index)
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


Boolean set_video_mode(Number video_mode_index)
{
	Number16 video_mode;
	
	video_mode = get_video_mode(video_mode_index);
	
	if(!video_mode) {
		return 0;
	}
	
	loader_api->set_VESA_mode(video_mode | 0x4000);
	
	return 1;
}


Boolean get_video_mode_info(Number video_mode_index, VESA_Mode_Info* video_mode_info)
{
	Number16 video_mode;
	
	video_mode = get_video_mode(video_mode_index);
	
	if(!video_mode) {
		return 0;
	}
	
	loader_api->get_VESA_mode_info(video_mode_info, video_mode | 0x4000);
	
	return 1;
}


VESA_Mode_Info video_mode_info;
Number         current_video_mode_info_index = -1;


void update_video_mode_info(Number video_mode_index)
{
	if(video_mode_index == current_video_mode_info_index) {
		return;
	}
	
	if(!get_video_mode_info(video_mode_index, &video_mode_info)) {
		return;
	}
	
	current_video_mode_info_index = video_mode_index;
}


Number get_video_mode_width(Number video_mode_index)
{
	update_video_mode_info(video_mode_index);
	return video_mode_info.width;
}


Number get_video_mode_pitch(Number video_mode_index)
{
	update_video_mode_info(video_mode_index);
	return video_mode_info.pitch;
}


Number get_video_mode_height(Number video_mode_index)
{
	update_video_mode_info(video_mode_index);
	return video_mode_info.height;
}


Number get_video_mode_bits_per_pixel(Number video_mode_index)
{
	update_video_mode_info(video_mode_index);
	return video_mode_info.bpp;
}


void* get_video_mode_framebuffer(Number video_mode_index)
{
	update_video_mode_info(video_mode_index);
	return video_mode_info.framebuffer;
}