Boolean load_COM_program(FAT_File_System* fs, Process* process, FAT_Data* file)
{
	Byte* program;
	
	process->start = allocate_memory(file->file_size + 64 * 1024);
	program = process->start;
	
	while(read_fs_file_sector(fs, file, program)) {
		program += 512;
	}
	
	process->esp = process->start + file->file_size + 64 * 1024;
	process->ebp = process->esp;

	return 1;
}