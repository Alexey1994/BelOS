#ifndef FILE_INCLUDED
#define FILE_INCLUDED


typedef enum {
	FAT_READ_ONLY = 0x01,
	FAT_HIDDEN    = 0x02,
	FAT_SYSTEM    = 0x04,
	FAT_VOLUME_ID = 0x08,
	FAT_DIRECTORY = 0x10,
	FAT_ARCHIVE   = 0x20,
	FAT_LONG_NAME = 0x0F
}
FAT_Data_Attribute;

typedef struct {
	Byte     name[11];
	Byte     attributes;
	Byte     zero;
	Number8  milliseconds_creation;
	Number16 time_created;
	Number16 date_created;
	Number16 last_access_date;
	Number16 cluster_number_high;
	Number16 time_last_write;
	Number16 date_last_write;
	Number16 cluster_number_low;
	Number32 file_size;
}
FAT_Data;


typedef struct {
	Number32  data_cluster;
	Number32  cluster_offset;
	Number32  file_number;
	FAT_Data* file_data;
	Byte      sector[512];
}
File_Enumerator;


#endif//FILE_INCLUDED