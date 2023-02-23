SECTIONS
{
	. = 0;
	. = ALIGN(4);
	.text : {
		* (.text);
	}
	.data : {
		* (.data);
	}
}