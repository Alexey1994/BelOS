#include <API.c>


API* _api;

Pipe_Interface* _pipe_interface;
File_Interface* _file_interface;
Heap_Interface* _heap_interface;
Text_Display_Interface* _text_display_interface;
Interrupt_Interface* _interrupt_interface;
PCI_Interface* _pci_interface;


Number main(Number number_of_arguments, Byte** arguments);


void print_error(Byte* parameters, ...);


void start(API* api)
{
	get_module_address_by_function(start);

	global(_api) = api;
	
	global(_pipe_interface) = api->get(global_ptr("pipe"));
	global(_file_interface) = api->get(global_ptr("file"));
	global(_heap_interface) = api->get(global_ptr("heap"));
	global(_text_display_interface) = api->get(global_ptr("display/text"));
	global(_interrupt_interface) = api->get(global_ptr("interrupt"));
	global(_pci_interface) = api->get(global_ptr("PCI"));
	
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
	
}
USB;

USB _usb;


#include <IO.c>
#include "host-controller/UHCI.c"
#include "host-controller/OHCI.c"
#include "host-controller/EHCI.c"
#include "host-controller/XHCI.c"


Number main(Number number_of_arguments, Byte** arguments)
{
	get_module_address_by_function(main);
	PCI_Interface* pci_interface = global(_pci_interface);
	USB* usb = global_ptr(_usb);
	API* api = global(_api);
	
	
	//if(initialize_ps2_mouse()) {
	//	mouse->interface.set_handler = global_ptr(set_mouse_handler);
	//	api->create("/mouse" + module_address, &mouse->interface);
	//}
	
	PCI_Config config;
	Number     i;
	
	for(i = 0; i < 65536; ++i) {
		if(pci_interface->read_PCI_config(i, &config)) {
			//print(global_ptr("found ven_%x%x dev_%x%x\n"), config.vendor >> 8, config.vendor, config.device >> 8, config.device);
			
			if(config.class == 12 && config.subclass == 3 && config.prog_IF == 0) {
				//print(global_ptr("found USB UHCI\n"));
				//configure_USB_UHCI(i, &config);
			}
			else if(config.class == 12 && config.subclass == 3 && config.prog_IF == 16) {
				//print(global_ptr("found USB OHCI\n"));
				//configure_USB_OHCI(i, &config);
			}
			else if(config.class == 12 && config.subclass == 3 && config.prog_IF == 32) {
				//print(global_ptr("found USB EHCI\n"));
				//configure_USB_EHCI(i, &config);
			}
			else if(config.class == 12 && config.subclass == 3 && config.prog_IF == 48) {
				//print(global_ptr("found USB XHCI\n"));
			}
		}
	}
}