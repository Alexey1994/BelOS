#ifndef STORAGE_FAT32_INIT_INCLUDED
#define STORAGE_FAT32_INIT_INCLUDED


#include <types.c>
#include <memory.c>

#include <file.c>


typedef struct {
	Byte     os_name[8];                 //имя того кто форматировал
	Number16 bytes_in_sector;            //512, 1024, 2048 или 4096
	Number8  sectors_in_cluster;         //1, 2, 4, 8, 16, 32, 64 или 128
	Number16 number_of_reserved_sectors; //любое число кроме 0
	Number8  number_of_FAT_tables;       //2 рекомендуется, 1 приемлемо
	Number32 zeros;
	Number8  type;                       //0xF8 - не перемещаемый
	Number16 zeros2;
	Number16 sectors_per_track;          //секторов в дорожке - 63
	Number16 number_of_heads;            //головок - 255
	Number32 number_of_hidden_sectors;   //0 если нет разделов
	Number32 number_of_sectors;          //не 0

	//FAT32 часть
	Number32 number_of_sectors_in_FAT_table;
	Number16 flags;                           //биты 0-3 - номер активного FAT, биты 4-6 - зарезервировано, бит 7 - 1 если один FAT активен, биты 8-15 - зарезервировано. Рекомендуется 0
	Number16 version;                         //0
	Number32 root_directory_cluster;          //обычно 2 (т.е. самый первый кластер data раздела)
	Number16 FSInfo_sector_number;            //обычно 1 (0 или 0xFFFF - нет FSInfo)
	Number16 backup_sector_number;            //0 или 6
	Byte     zeros3[12];
	Number8  drive_number;                    //0x00 или 0x80
	Byte     zeros4;
	Number8  signature;                       //0x29
	Number32 volume_serial_number;            //генерируется как время создания
	Byte     volume_label[11];                //имя раздела
	Byte     file_system_name[8];             //FAT32 и 3 пробела
}
FAT32;

typedef struct {
	Number32 signature1;              //0x41615252
	Byte     zeros[480];
	Number32 signature2;              //0x61417272
	Number32 number_of_free_clusters; //0xFFFFFFFF - неизвестно
	Number32 next_free_cluster;       //0xFFFFFFFF - нет информации о следующем свободном кластере
	Byte     zeros2[12];
	Number32 signature3;              //0xAA550000
}
FAT32_FSInfo;


typedef struct {
	Boolean (*read_sector)(Number sector_number, Byte* sector);

	Number32 FAT_start_sector;
	Number32 data_start_sector;
	Number32 sectors_in_cluster;
	Number32 number_of_clusters;

	FAT_Data current_directory;

	Number32 current_FAT_sector;
	Number32 FAT[128];
}
FAT_File_System;


void open_FAT_File_System(
	FAT_File_System* fs,
	Boolean (*read_sector)(Number sector_number, Byte* sector)
)
{
	Byte     sector[512];
	FAT32*   fat32;

	fs->read_sector = read_sector;

	fs->read_sector(1, sector);
	fat32 = sector + 3;

	fs->FAT_start_sector          = fat32->number_of_hidden_sectors + fat32->number_of_reserved_sectors;
	fs->data_start_sector         = fs->FAT_start_sector + fat32->number_of_FAT_tables * fat32->number_of_sectors_in_FAT_table;
	fs->sectors_in_cluster        = fat32->sectors_in_cluster;
	fs->number_of_clusters        = fat32->number_of_sectors / fat32->sectors_in_cluster;

	clean_bytes(&fs->current_directory, sizeof(fs->current_directory));
	fs->current_directory.cluster_number_low = fat32->root_directory_cluster;
	fs->current_directory.cluster_number_high = fat32->root_directory_cluster >> 16;

	fs->current_FAT_sector = 0;
	fs->read_sector(fs->FAT_start_sector + fs->current_FAT_sector, fs->FAT);
}


Number32 next_FAT_cluster(FAT_File_System* fs, Number32 current_cluster)
{
	if(current_cluster / 128 != fs->current_FAT_sector) {
		fs->current_FAT_sector = current_cluster / 128;
		
		if(!fs->read_sector(fs->FAT_start_sector + fs->current_FAT_sector, fs->FAT)) {
			print("next_FAT_cluster error\n");
		}
	}

	return fs->FAT[current_cluster % 128];
}


void read_FAT_sector(FAT_File_System* fs, Number32 cluster_number, Byte* sector, Number32 offset)
{
	//print("{%d}", fs->sectors_in_cluster);
	
	if(!fs->read_sector(fs->data_start_sector + (cluster_number - 2) * fs->sectors_in_cluster + offset, sector)) {
		print("read_FAT_sector error\n");
	}
}


#endif//STORAGE_FAT32_INIT_INCLUDED