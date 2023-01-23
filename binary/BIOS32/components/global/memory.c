#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED


void clean_bytes(Byte* bytes, Number size)
{
	Number system_size;
	Number remind_size;

	system_size = size / sizeof(Number);
	remind_size = size % sizeof(Number);

	while(system_size) {
		*((Number*)bytes) = 0;

		--system_size;
		bytes += sizeof(Number);
	}

	while(remind_size) {
		*bytes = 0;

		--remind_size;
		++bytes;
	}
}



void copy_bytes(Byte* destination, Byte* source, Number size)
{
	Number system_size;
	Number remind;

	system_size = size / sizeof(Number);
	remind = size % sizeof(Number);

	while(system_size) {
		*((Number*)destination) = *((Number*)source);

		--system_size;
		destination += sizeof(Number);
		source += sizeof(Number);
	}

	while(remind) {
		*destination = *source;

		--remind;
		++destination;
		++source;
	}
}


Signed_Number compare_null_terminated_bytes(Byte* bytes1, Byte* bytes2)
{
	Signed_Number difference;

	while(*bytes1 && *bytes2) {
		difference = (Signed_Number)*bytes1 - (Signed_Number)*bytes2;

		if(difference) {
			return difference;
		}

		++bytes1;
		++bytes2;
	}

	return (Signed_Number)*bytes1 - (Signed_Number)*bytes2;
}


Signed_Number compare_bytes(Byte* bytes1, Number number_of_bytes1, Byte* bytes2, Number number_of_bytes2)
{
	Signed_Number difference;

	while(number_of_bytes1 && number_of_bytes2) {
		difference = (Signed_Number)*bytes1 - (Signed_Number)*bytes2;

		if(difference) {
			return difference;
		}

		++bytes1;
		++bytes2;
		--number_of_bytes1;
		--number_of_bytes2;
	}

	return (Signed_Number)number_of_bytes1 - (Signed_Number)number_of_bytes2;
}


#endif//MEMORY_INCLUDED