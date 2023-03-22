#include <API.c>


import extern API* api;


void _start(API* api)
{
	api->display.text.write_character(0, 'H');
	api->display.text.write_character(0, 'i');
}