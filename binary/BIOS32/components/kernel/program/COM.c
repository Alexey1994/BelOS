Process_Start load_COM_program(FAT_Data* file, API* api)
{
	Process_Start start;
	Byte*         program;

	start = allocate_memory(256 * 1024);
	program = start;
	
	while(read_FAT_file_sector(&fs, file, program)) {
		program += 512;
	}

	return start;
}