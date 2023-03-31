#include <API.c>


import extern API* api;


void _start(API* api)
{
	((Text_Display_Interface*)api->get("display/text"))->write_character(0, 'H');
	((Text_Display_Interface*)api->get("display/text"))->write_character(0, 'i');
}