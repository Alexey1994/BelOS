typedef enum
{
	UHCI_COMMAND                  = 0,  //2 bytes
	UHCI_STATUS                   = 2,  //2 bytes
	UHCI_INTERRUPT                = 4,  //2 bytes
	UHCI_FRAME_NUMBER             = 6,  //2 bytes
	UHCI_FRAME_LIST_BASE_ADDRESS  = 8,  //4 bytes
	UHCI_START_OF_FRAME_MODIFY    = 12, //1 byte
	UHCI_PORT1_STATUS_AND_CONTROL = 16, //2 bytes
	UHCI_PORT2_STATUS_AND_CONTROL = 18  //2 bytes
}
UHCI_Register;


typedef enum
{
	UHCI_COMMAND_RUN              = 1,
	UHCI_COMMAND_CONTROLLER_RESET = 2,
	UHCI_COMMAND_GLOBAL_RESET     = 4,
	UHCI_COMMAND_GLOBAL_SUSPEND   = 8,
	UHCI_COMMAND_GLOBAL_RESUME    = 16,
	UHCI_COMMAND_SOFTWARE_DEBUG   = 32,
	UHCI_COMMAND_CONFIGURE        = 64,
	UHCI_COMMAND_MAX_PACKET       = 128
}
UHCI_Commands;


typedef enum
{
	UHCI_STATUS_INTERRUPT       = 1,
	UHCI_STATUS_ERROR_INTERRUPT = 2,
	UHCI_STATUS_RESUME_DETECTED = 4,
	UHCI_STATUS_SYSTEM_ERROR    = 8,
	UHCI_STATUS_PROCESS_ERROR   = 16,
	UHCI_STATUS_HALTED          = 32
}
UHCI_Statuses;


typedef enum
{
	UHCI_INTERRUPT_TIMEOUT_CRC  = 1,
	UHCI_INTERRUPT_RESUME       = 2,
	UHCI_INTERRUPT_COMPLETE     = 4,
	UHCI_INTERRUPT_SHORT_PACKET = 8
}
UHCI_Interrupt;


typedef enum
{
	UHCI_PORT_CONNECTED            = 1,
	UHCI_PORT_CONNECTED_CHANGED    = 2,
	UHCI_PORT_PORT_ENABLED         = 4,
	UHCI_PORT_PORT_ENABLED_CHANGED = 8,
	UHCI_PORT_LINE_STATUS1         = 16,
	UHCI_PORT_LINE_STATUS2         = 32,
	UHCI_PORT_RESUME_DETECTED      = 64,
	UHCI_PORT_ONE                  = 128,
	UHCI_PORT_LOW_SPEED            = 256,
	UHCI_PORT_RESET                = 512,
	UHCI_PORT_SUSPEND              = 4096
}
UHCI_Port;


void configure_USB_UHCI(Number16 addresss, PCI_Config* config)
{
	get_module_address_by_function(configure_USB_UHCI);
	PCI_Interface* pci_interface = global_ptr(_pci_interface);
	
	
	Number16 io_base;

	io_base = pci_interface->get_PCI_IO_address(config, 4);

	//disable interrupts
	out_16(io_base + UHCI_INTERRUPT, 0);

	//clear status
	out_16(io_base + UHCI_STATUS, 0xFFFF);

	//enable controller
	out_16(io_base + UHCI_COMMAND, UHCI_COMMAND_CONTROLLER_RESET);

	//out_16(io_base + UHCI_USB_COMMAND, in_16(io_base + UHCI_USB_COMMAND) | UHCI_COMMAND_RUN);
	out_16(io_base + UHCI_PORT1_STATUS_AND_CONTROL, 0);

	
	print(global_ptr("%d\n"), in_16(io_base + UHCI_COMMAND_CONTROLLER_RESET));
}