#include <API.c>


API* _api;

Pipe_Interface* _pipe_interface;
File_Interface* _file_interface;
Text_Display_Interface* _text_display_interface;
Interrupt_Interface* _interrupt_interface;


Number main(Number number_of_arguments, Byte** arguments);


void print_error(Byte* parameters, ...);


void start(API* api)
{
	get_module_address_by_function(start);

	global(_api) = api;
	
	global(_pipe_interface) = api->get(global_ptr("pipe"));
	global(_file_interface) = api->get(global_ptr("file"));
	global(_text_display_interface) = api->get(global_ptr("display/text"));
	global(_interrupt_interface) = api->get(global_ptr("interrupt"));
	
	main(api->number_of_arguments, api->arguments);
}


#include <memory.c>
#include <writer.c>


void print(Byte* parameters, ...)
{
	get_module_address_by_function(print);
	Pipe_Interface* pipe_interface = global(_pipe_interface);
	
	print_in_source(0, pipe_interface->write_character, parameters, &parameters + 1);
}


void print_error(Byte* parameters, ...)
{
	get_module_address_by_function(print_error);
	Text_Display_Interface* text_display_interface = global(_text_display_interface);
	
	print_in_source(0, text_display_interface->write_character, parameters, &parameters + 1);
}




typedef struct {
	PCI_Interface interface;
}
PCI;

PCI _pci;


#include <PCI.c>
#include <IO.c>


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


Number main(Number number_of_arguments, Byte** arguments)
{
	get_module_address_by_function(main);
	PCI* pci = global_ptr(_pci);
	API* api = global(_api);
	
	
	pci->interface.read_byte_from_PCI_config   = global_ptr(read_byte_from_PCI_config);
	pci->interface.read_2bytes_from_PCI_config = global_ptr(read_2bytes_from_PCI_config);
	pci->interface.read_4bytes_from_PCI_config = global_ptr(read_4bytes_from_PCI_config);
	pci->interface.read_bytes_from_PCI_config  = global_ptr(read_bytes_from_PCI_config);
	pci->interface.write_byte_in_PCI_config    = global_ptr(write_byte_in_PCI_config);
	pci->interface.write_2bytes_in_PCI_config  = global_ptr(write_2bytes_in_PCI_config);
	pci->interface.write_4bytes_in_PCI_config  = global_ptr(write_4bytes_in_PCI_config);
	pci->interface.write_bytes_in_PCI_config   = global_ptr(write_bytes_in_PCI_config);
	pci->interface.read_PCI_config             = global_ptr(read_PCI_config);
	pci->interface.get_PCI_MMIO_address        = global_ptr(get_PCI_MMIO_address);
	pci->interface.get_PCI_IO_address          = global_ptr(get_PCI_IO_address);
	
	api->create(global_ptr("PCI"), &pci->interface);
}