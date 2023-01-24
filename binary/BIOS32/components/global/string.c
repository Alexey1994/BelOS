#ifndef STRING_INCLUDED
#define STRING_INCLUDED


#include <types.c>


Byte to_upper_case(Byte c)
{
	if(c >= 'a' && c <= 'z') {
		c = c + 'A' - 'a';
	}
	
	return c;
}


#endif//STRING_INCLUDED