SECTIONS
{
	. = 0x100000;
	. = ALIGN(4);
	.text : {
		* (.text);
	}
	.data : {
		* (.data);
	}
}