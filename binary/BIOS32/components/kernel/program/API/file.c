#include "FAT/read.c"


FAT_File_System root_fs;


void open_fs_root(FAT_File_System* fs)
{
	open_FAT_File_System(fs, loader_api->read_sector);
}


Boolean enum_fs_files(FAT_File_System* fs, File_Enumerator* enumerator)
{
	if(!enumerator->data_cluster) {
		enumerator->data_cluster = fs->current_directory.cluster_number_low + (fs->current_directory.cluster_number_high << 16);
		enumerator->cluster_offset = 0;
		enumerator->file_number = 0;
	}
	
	return enum_FAT_data(fs, enumerator);
}


Boolean open_fs_directory(FAT_File_System* fs, Byte* name)
{
	if(name[0] == '/') {
		open_fs_root(fs);
		return 1;
	}
	
	return open_FAT_directory(fs, name);
}


Boolean open_fs_file(FAT_File_System* fs, FAT_Data* file, Byte* name)
{
	return open_FAT_file(fs, file, name);
}


Number read_fs_file_sector(FAT_File_System* fs, FAT_Data* file, Byte* sector)
{
	return read_FAT_file_sector(fs, file, sector);
}