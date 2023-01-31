#ifndef STORAGE_FAT32_READ_INCLUDED
#define STORAGE_FAT32_READ_INCLUDED


#include "init.c"
#include <string.c>


Boolean enum_FAT_data(FAT_File_System* fs, File_Enumerator* enumerator)
{
	if(enumerator->data_cluster == 0x0FFFFFFF) {
		return 0;
	}
	
	if(!enumerator->file_number) {
		read_FAT_sector(fs, enumerator->data_cluster, enumerator->sector, enumerator->cluster_offset);
	}

	enumerator->file_data = enumerator->sector + enumerator->file_number * sizeof(FAT_Data);

	++enumerator->file_number;
	
	if(enumerator->file_number >= 512 / sizeof(FAT_Data)) {
		enumerator->file_number = 0;
		++enumerator->cluster_offset;
		
		if(enumerator->cluster_offset >= fs->sectors_in_cluster) {
			enumerator->cluster_offset = 0;
			enumerator->data_cluster = next_FAT_cluster(fs, enumerator->data_cluster);
		}
	}
	
	return enumerator->file_data->name[0];
}


Boolean open_FAT_data(FAT_File_System* fs, Byte* data_name, Number32 data_cluster, FAT_Data* data)
{
	File_Enumerator enumerator;
	Byte            name[11];
	Number          i;
	
	for(i = 0; i < 11; ++i) {
		if(*data_name && *data_name != '.') {
			name[i] = to_upper_case(*data_name);
			++data_name;
		}
		else {
			name[i] = ' ';
		}
	}
	
	if(*data_name == '.') {
		++data_name;
		
		for(i = 8; i < 11 && *data_name; ++i) {
			name[i] = to_upper_case(*data_name);
			++data_name;
		}
	}
	
	enumerator.data_cluster   = data_cluster;
	enumerator.cluster_offset = 0;
	enumerator.file_number    = 0;
	
	while(enum_FAT_data(fs, &enumerator)) {
		if(!compare_bytes(enumerator.file_data->name, 11, name, 11)) {
			copy_bytes(data, enumerator.file_data, sizeof(FAT_Data));
			return 1;
		}
	}
	
	return 0;
}


Boolean open_FAT_file(FAT_File_System* fs, FAT_Data* file, Byte* name)
{
	return open_FAT_data(
		fs,
		name,
		fs->current_directory.cluster_number_low + (fs->current_directory.cluster_number_high << 16),
		file
	) && !(file->attributes & FAT_DIRECTORY);
}


Boolean open_FAT_directory(FAT_File_System* fs, Byte* name)
{
	FAT_Data directory;
	
	if(
		!open_FAT_data(
			fs,
			name,
			fs->current_directory.cluster_number_low + (fs->current_directory.cluster_number_high << 16),
			&directory
		) || !(directory.attributes & FAT_DIRECTORY)
	) {
		return 0;
	}

	copy_bytes(&fs->current_directory, &directory, sizeof(FAT_Data));

	return 1;
}


Number read_FAT_file_sector(FAT_File_System* fs, FAT_Data* file, Byte* sector)
{
	Number32 cluster_number;
	
	cluster_number = file->cluster_number_low + (file->cluster_number_high << 16);
	
	if(cluster_number == 0x0FFFFFFF) {
		return 0;
	}
	
	read_FAT_sector(fs, cluster_number, sector, 0);
	cluster_number = next_FAT_cluster(fs, cluster_number);
	
	file->cluster_number_low = cluster_number & 0xFFFF;
	file->cluster_number_high = cluster_number >> 16;
	
	return 512;
}


#endif//STORAGE_FAT32_READ_INCLUDED