#include <API.c>


import extern API* api;


void _start(API* api)
{
	api->print("Hi");
}