#ifndef PCI_INCLUDED
#define PCI_INCLUDED


/*
   | bits 31-24  | bits 23-16  |  bits 15-8    | bits 7-0             |
-----------------------------------------------------------------------
00 | Device ID                 |  Vendor ID                           |
   --------------------------------------------------------------------
04 | Status                    |  Command                             |
   --------------------------------------------------------------------
08 | Class code  | Subclass    |  Prog IF      | Revision ID          |
   --------------------------------------------------------------------
0C | BIST        | Header type | Latency Timer | Cache Line Size      |
   --------------------------------------------------------------------
10 | Base address #0 (BAR0)                                           |
   --------------------------------------------------------------------
14 | Base address #1 (BAR1)                                           |
   --------------------------------------------------------------------
18 | Base address #2 (BAR2)                                           |
   --------------------------------------------------------------------
1C | Base address #3 (BAR3)                                           |
   --------------------------------------------------------------------
20 | Base address #4 (BAR4)                                           |
   --------------------------------------------------------------------
24 | Base address #5 (BAR5)                                           |
   --------------------------------------------------------------------
28 | Cardbus CIS Pointer                                              |
   --------------------------------------------------------------------
2C | Subsystem ID              | Subsystem Vendor ID                  |
   --------------------------------------------------------------------
30 | Expansion ROM base address                                       |
   --------------------------------------------------------------------
34 | Reserved                                  | Capabilities Pointer |
   --------------------------------------------------------------------
38 | Reserved                                                         |
   --------------------------------------------------------------------
3C | Max latency | Min Grant   | Interrupt PIN | Interrupt Line       |
   -------------------------------------------------------------------- 
*/

/*
typedef struct {
	Number16 address; //bus << 8 | slot << 3 | func; bus[0..256], slot[0..32], func[0..8]

	struct {
		Number16 vendor;
		Number16 device;
		Number16 command;
		Number16 status;
		Number8  revision;
		Number8  prog_IF;
		Number8  subclass;
		Number8  class;
		Number8  cache_line_size;
		Number8  latency_timer;
		Number8  header_type;
		Number8  BIST;
		Number32 BARs[6];
		Number32 cardbus_CIS_pointer;
		Number16 subsystem_vendor;
		Number16 subsystem;
		Number32 expansion_ROM_base_address;
		Number8  capabilities_pointer;
		Number8  reserved1;
		Number16 reserved2;
		Number32 reserved3;
		Number8  interrupt_line;
		Number8  interrupt_pin;
		Number8  min_grant;
		Number8  max_latency;
	}
	config;
}
PCI_Device;*/


typedef enum {
	PCI_VENDOR_ID           = 0,
	PCI_DEVICE_ID           = 2,
	PCI_COMMAND             = 4,
	PCI_STATUS              = 6,
	PCI_REVISION_ID         = 8,
	PCI_PROG_IF             = 9,
	PCI_SUBCLASS            = 10,
	PCI_CLASS_CODE          = 11,
	PCI_CACHE_LINE_SIZE     = 12,
	PCI_LATENCY_TIMER       = 13,
	PCI_HEADER_TYPE         = 14,
	PCI_BIST                = 15,
	PCI_BAR0                = 16,
	PCI_BAR1                = 20,
	PCI_BAR2                = 24,
	PCI_BAR3                = 28,
	PCI_BAR4                = 32,
	PCI_BAR5                  = 36,
	PCI_CARDBUS_CIS_POINTER   = 40,
	PCI_SUBSYSTEM_VENDOR_ID   = 44,
	PCI_SUBSYSTEM_ID          = 46,
	PCI_EXPANSION_ROM_ADDRESS = 48,
	PCI_CAPABILITIES_POINTER  = 52,
	PCI_INTERRUPT_LINE        = 60,
	PCI_INTERRUPT_PIN         = 61,
	PCI_MIN_GRANT             = 62,
	PCI_MAX_LATENCY           = 63
}
PCI_Field;


typedef struct {
	Number16 vendor;
	Number16 device;
	Number16 command;
	Number16 status;
	Number8  revision;
	Number8  prog_IF;
	Number8  subclass;
	Number8  class;
	Number8  cache_line_size;
	Number8  latency_timer;
	Number8  header_type;
	Number8  BIST;
	Number32 BARs[6];
	Number32 cardbus_CIS_pointer;
	Number16 subsystem_vendor;
	Number16 subsystem;
	Number32 expansion_ROM_base_address;
	Number8  capabilities_pointer;
	Number8  reserved1;
	Number16 reserved2;
	Number32 reserved3;
	Number8  interrupt_line;
	Number8  interrupt_pin;
	Number8  min_grant;
	Number8  max_latency;
}
PCI_Config;


#include "IO.c"


void set_PCI_config_address(Number16 address, Number8 offset)
{
	out_32(
		0xCF8,
		0b10000000000000000000000000000000 //enable bit
			| (address << 8)
			| offset
	);
}


Byte read_byte_from_PCI_config(Number16 address, Number8 offset)
{
	set_PCI_config_address(address, offset);
	return in_8(0xCFC);
}


Number16 read_2bytes_from_PCI_config(Number16 address, Number8 offset)
{
	set_PCI_config_address(address, offset);
	return in_16(0xCFC);
}


Number32 read_4bytes_from_PCI_config(Number16 address, Number8 offset)
{
	set_PCI_config_address(address, offset);
	return in_32(0xCFC);
}


void read_bytes_from_PCI_config(Number16 address, Number8 offset, Byte* bytes, Number8 size)
{
	while(size >= 4) {
		*(Number32*)bytes = read_4bytes_from_PCI_config(address, offset);
		
		size -= 4;
		offset += 4;
		bytes += 4;
	}
	
	while(size >= 2) {
		*(Number16*)bytes = read_2bytes_from_PCI_config(address, offset);
		
		size -= 2;
		offset += 2;
		bytes += 2;
	}
	
	while(size >= 1) {
		*bytes = read_byte_from_PCI_config(address, offset);
		
		--size;
		++offset;
		++bytes;
	}
}


void write_byte_in_PCI_config(Number16 address, Number8 offset, Byte byte)
{
	set_PCI_config_address(address, offset);
	out_8(0xCFC, byte);
}


void write_2bytes_in_PCI_config(Number16 address, Number8 offset, Number16 bytes)
{
	set_PCI_config_address(address, offset);
	out_16(0xCFC, bytes);
}


void write_4bytes_in_PCI_config(Number16 address, Number8 offset, Number32 bytes)
{
	set_PCI_config_address(address, offset);
	out_32(0xCFC, bytes);
}


void write_bytes_in_PCI_config(Number16 address, Number8 offset, Byte* bytes, Number8 size)
{
	while(size >= 4) {
		write_4bytes_in_PCI_config(address, offset, *(Number32*)bytes);
		
		size -= 4;
		offset += 4;
		bytes += 4;
	}
	
	while(size >= 2) {
		write_2bytes_in_PCI_config(address, offset, *(Number16*)bytes);
		
		size -= 2;
		offset += 2;
		bytes += 2;
	}
	
	while(size >= 1) {
		write_byte_in_PCI_config(address, offset, *bytes);
		
		--size;
		++offset;
		++bytes;
	}
}


Boolean read_PCI_config(Number16 address, PCI_Config* config)
{
	read_bytes_from_PCI_config(address, 0, config, 4);
	
	if(config->vendor == 0xFFFF && config->device == 0xFFFF) {
		return 0;
	}
		
	read_bytes_from_PCI_config(address, 4, (Byte*)config + 4, sizeof(PCI_Config) - 4);
	
	return 1;
}


Number32 get_PCI_MMIO_address(PCI_Config* config, Number BAR_number)
{
	return config->BARs[BAR_number] & 0b11111111111111111111111111110000;
}


Number32 get_PCI_IO_address(PCI_Config* config, Number BAR_number)
{
	return config->BARs[BAR_number] >> 2;
}


#endif//PCI_INCLUDED