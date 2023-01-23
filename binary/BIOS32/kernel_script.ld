SECTIONS
{
	. = 0x8000;
	. = ALIGN(4);
	.text : {
		INCLUDE "bin/components/kernel16.ld";
		* (.text);
	}
	.data : {
		* (.data);
	}
}