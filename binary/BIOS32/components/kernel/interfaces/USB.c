#ifndef USB_INCLUDED
#define USB_INCLUDED


#include "PCI.c"


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
	Number16 io_base;

	io_base = get_PCI_IO_address(config, 4);

	//disable interrupts
	out_16(io_base + UHCI_INTERRUPT, 0);

	//clear status
	out_16(io_base + UHCI_STATUS, 0xFFFF);

	//enable controller
	out_16(io_base + UHCI_COMMAND, UHCI_COMMAND_CONTROLLER_RESET);

	//out_16(io_base + UHCI_USB_COMMAND, in_16(io_base + UHCI_USB_COMMAND) | UHCI_COMMAND_RUN);
	out_16(io_base + UHCI_PORT1_STATUS_AND_CONTROL, 0);

	
	print("%d\n", in_16(io_base + UHCI_COMMAND_CONTROLLER_RESET));
}


//////////////////////////////////////////////////////////////////


typedef struct
{
	//hardware
	Number32 interrupts[32];
	Number16 frame_number;
	Number16 pad1;
	Number32 done_head;
	//Byte     reserved[120];
/*
	//software
	OHCI_Endpoint endpoints[63];
	OHCI_Endpoint control_endpoint;
	OHCI_Endpoint bulk_endpoint;
	Number32      mmio_base;
	Number32      powered_up;
	Number32      done_list;
	Number32      done_list_end_address;
	Number32      EHCI_companion;*/
}
OHCI_HCCA; //host controller communication area


typedef enum
{
	OHCI_HOST_CONTROLLER_RESET = 1,
	OHCI_CONTROL_LIST_FILLED = 2,
	OHCI_BULK_LIST_FILLED = 4
}
OHCI_Command_Status;


typedef enum
{
	OHCI_INTERRUPT_MASTER_ENABLE = 0x80000000
}
OHCI_Interrupt;


typedef struct
{
	/*struct
	{
		Number32 function_address: 7;
		Number32 endpoint_number: 4;
		Number32 direction: 2; //(1 - out, 2 - in), если 0 или 3 то определено в PID transfer descriptor
		Number32 speed: 1; //0 - full speed, 1 - low speed
		Number32 sKip: 1;
		Number32 format: 1; //0 for control, bulk, interrupt, 1 for isochronous endpoints
		Number32 maximum_packet_size: 11;
		Number32 reserved: 5;
	}
	flags;*/
	Number32 flags;
	Number32 tail;
	Number32 head;
	Number32 next;
}
OHCI_Endpoint;


typedef struct
{
	/*struct
	{
		Number32 reserved: 18;
		Number32 buffer_rounding: 1;
		Number32 direction_PID: 2;
		Number32 delay_interrupt: 3;
		Number32 data_toggle: 2;
		Number32 error_count: 2;
		Number32 condition_code: 4;
	}
	flags;*/
	Number32 flags;
	Number32 first_byte_address;
	Number32 next;
	Number32 last_byte_address;
}
OHCI_Transfer;


typedef struct
{
	Number32 flags;
	Number32 first_byte_address;
	Number32 next;
	Number32 last_byte_address;
	Number16 offset[8];
}
OHCI_Isochronous_Transfer;

/*
typedef enum
{
	OHCI_HOST_RESET = 0,
	OHCI_HOST_RESUME = 1,
	OHCI_HOST_OPERATIONAL = 2,
	OHCI_HOST_SUSPEND = 3
}
OHCI_Command_Status;*/


#define OHCI_FRAME_INTERVAL 0x2EDF
#define OHCI_DEFAULT_FRAME_INTERVAL ((((6 * (OHCI_FRAME_INTERVAL - 210)) / 7) << 16) | OHCI_FRAME_INTERVAL)

#define OHCI_PERIODIC_START 0x00002A27


#define OHCI_HC_STATE_RESET       0
#define OHCI_HC_STATE_RESUME      1
#define OHCI_HC_STATE_OPERATIONAL 2
#define OHCI_HC_STATE_SUSPEND     3


typedef struct
{
	//control and status
	Number32 revision;
	/*struct
	{
		Number32 control_bulk_service_ratio: 2;
		Number32 periodic_list_enable: 1;
		Number32 isochronus_enable: 1;
		Number32 control_list_enable: 1;
		Number32 bulk_list_enable: 1;
		Number32 host_controller_functional_state: 2;
		Number32 interrupt_routing: 1;
		Number32 remoute_wakeup_connected: 1;
		Number32 remote_wakeup_enable: 1;
		Number32 reserved: 21;
	}*/
	Number32 control;
	struct
	{
		Number32 host_controller_reset: 1;
		Number32 control_list_filled: 1;
		Number32 bulk_list_filled: 1;
		Number32 ownership_change_request: 1;
		Number32 reserved1: 12;
		Number32 scheduling_overrun_count: 1;
		Number32 reserved2: 15;
	}
	command_status;
	struct
	{
		Number32 scheduling_overrun: 1;
		Number32 writeback_done_head: 1;
		Number32 start_of_frame: 1;
		Number32 resume_detected: 1;
		Number32 untercoverable_error: 1;
		Number32 frame_number_overflow: 1;
		Number32 root_hub_status_change: 1;
		Number32 reserved1: 23;
		Number32 ownership_change: 1;
		Number32 reserved2: 1;
	}
	interrupt_status;
	/*struct
	{
		Number32 scheduling_overrun: 1;
		Number32 writeback_done_head: 1;
		Number32 start_of_frame: 1;
		Number32 resume_detected: 1;
		Number32 untercoverable_error: 1;
		Number32 frame_number_overflow: 1;
		Number32 root_hub_status_change: 1;
		Number32 reserved1: 23;
		Number32 ownership_change: 1;
		Number32 master_interrupt_enable: 1;
	}*/
	Number32 interrupt_enable;
	/*struct
	{
		Number32 scheduling_overrun: 1;
		Number32 writeback_done_head: 1;
		Number32 start_of_frame: 1;
		Number32 resume_detected: 1;
		Number32 untercoverable_error: 1;
		Number32 frame_number_overflow: 1;
		Number32 root_hub_status_change: 1;
		Number32 reserved1: 23;
		Number32 ownership_change: 1;
		Number32 master_interrupt_enable: 1;
	}*/
	Number32 interrupt_disable;

	//memory pointer
	Number32 HCCA;
	Number32 PeriodCurrentED;
	Number32 ControlHeadED;
	Number32 ControlCurrentED;
	Number32 BulkHeadED;
	Number32 BulkCurrentED;
	Number32 DoneHead;

	//frame counter
	/*struct
	{
		Number32 interval: 14;
		Number32 reserved: 2;
		Number32 largest_data_packet: 15;
		Number32 toggle: 1;
	}*/
	Number32 frame_interval;
	Number32 frame_remaining;
	Number32 frame_number;
	Number32 PeriodicStart;
	Number32 LSThreshold;

	//root hub
	struct
	{
		Number32 number_of_downstream_ports: 8;
		Number32 power_switching_mode: 1;
		Number32 no_power_switching: 1;
		Number32 device_type: 1;
		Number32 over_current_protection_mode: 1;
		Number32 over_no_current_protection: 1;
		Number32 reserved: 11;
		Number32 power_on_to_power_good_time: 8;
	}
	root_hub_descriptor_A; //status?
	Number32 root_hub_descriptor_B;
	/*union
	{
		struct
		{
			Number32 local_power_status: 1;
			Number32 over_current_indicator: 1;
			Number32 reserved10: 13;
			Number32 device_remote_wakeup_enable: 1;
			Number32 local_power_sttus_change: 1;
			Number32 over_current_indicator_change_R: 1;
			Number32 reserved20: 14;
		};
		struct
		{
			Number32 clear_global_power: 1;
			Number32 reserved11: 14;
			Number32 start_remote_wakeup_enable: 1;
			Number32 set_global_power: 1;
			Number32 over_current_indicator_changeW: 1;
			Number32 reserved22: 13;
			Number32 clear_remote_wakeup_enable: 1;
		};
	}*/
	Number32 root_hub_status;

	/*
	struct
	{
		union
		{
			struct // read
			{
				USHORT  CurrentConnectStatus     : 1;
				USHORT  PortEnableStatus         : 1;
				USHORT  PortSuspendStatus        : 1;
				USHORT  PortOverCurrentIndicator : 1;
				USHORT  PortResetStatus          : 1;
				USHORT  Reserved1r               : 3;
				USHORT  PortPowerStatus          : 1;
				USHORT  LowSpeedDeviceAttached   : 1;
				USHORT  Reserved2r               : 6;
			};
			struct // write
			{
				USHORT  ClearPortEnable    : 1;
				USHORT  SetPortEnable      : 1;
				USHORT  SetPortSuspend     : 1;
				USHORT  ClearSuspendStatus : 1;
				USHORT  SetPortReset       : 1;
				USHORT  Reserved1w         : 3;
				USHORT  SetPortPower       : 1;
				USHORT  ClearPortPower     : 1;
				USHORT  Reserved2w         : 6;
			};
		};
		USHORT ConnectStatusChange            : 1;
		USHORT PortEnableStatusChange         : 1;
		USHORT PortSuspendStatusChange        : 1;
		USHORT PortOverCurrentIndicatorChange : 1;
		USHORT PortResetStatusChange          : 1;
		USHORT Reserved3                      : 11;
	};*/
	Number32 port_statuses[15]; //1 - порт подключен
}
OHCI_Interface;


OHCI_Interface* interface;
OHCI_HCCA*      hcca;


asm("pusha");
asm("call OHCI_interrupt_handler");
asm("mov $0x20, %al");
asm("out %al, $0x20");
asm("popa");
asm("iret");
void OHCI_interrupt_handler()
{
	print("OHCI interrupt\n");
	
	/*if(operation->status & (1 << 0)) {
		print("transfer\n");
	}
	if(operation->status & (1 << 1)) {
		print("error\n");
	}
	if(operation->status & (1 << 2)) {
		print("port_change_detect\n");
	}
	if(operation->status & (1 << 4)) {
		print("host_system_error\n");
	}
	if(operation->status & (1 << 5)) {
		print("doorbell_interrupt\n");
	}*/
	
	//if(operation->status & (1 << 2)) { //port_change_detect
		Number i;
		Number number_of_ports;
		
		number_of_ports = interface->root_hub_descriptor_A.number_of_downstream_ports;
		//print("%d downstream ports\n", interface->root_hub_descriptor_A.number_of_downstream_ports);
		
		for(i = 0; i < number_of_ports; ++i) {
			//print("port %d status %d\n", i, operation->port_status_and_control[i]);
			print("port %d status", i);
			
			Number32 port_status = interface->port_statuses[i];
			
			/*
				USHORT  CurrentConnectStatus     : 1;
				USHORT  PortEnableStatus         : 1;
				USHORT  PortSuspendStatus        : 1;
				USHORT  PortOverCurrentIndicator : 1;
				USHORT  PortResetStatus          : 1;
				USHORT  Reserved1r               : 3;
				USHORT  PortPowerStatus          : 1;
				USHORT  LowSpeedDeviceAttached   : 1;
				USHORT  Reserved2r               : 6;*/
			
			if(port_status & 1) {
				print(" connected");
			}
			
			if(port_status & (1 << 1)) {
				print(" enabled");
			}
			
			if(port_status & (1 << 2)) {
				print(" suspend");
			}
			
			if(port_status & (1 << 3)) {
				print(" over_current");
			}
			
			if(port_status & (1 << 4)) {
				print(" reset");
			}
			
			print("\n");
		}
	//}
	
	print("frame index: %d\n", hcca->frame_number);
}


void configure_USB_OHCI(Number16 addresss, PCI_Config* config)
{
	interface = get_PCI_MMIO_address(config, 0);
	hcca = interface->HCCA;
	
	print("USB %x\n", interface->revision);
	
	if(config->interrupt_line >= 0 && config->interrupt_line < 16) {
		loader_api->set_interrupt_handler((Number)&OHCI_interrupt_handler - 12, config->interrupt_line + 32);
	}
	else {
		print("no interrupts, interrupt_line is %d\n", config->interrupt_line);
	}
	
		/*Number32 scheduling_overrun: 1;
		Number32 writeback_done_head: 1;
		Number32 start_of_frame: 1;
		Number32 resume_detected: 1;
		Number32 untercoverable_error: 1;
		Number32 frame_number_overflow: 1;
		Number32 root_hub_status_change: 1;
		Number32 reserved1: 23;
		Number32 ownership_change: 1;
		Number32 master_interrupt_enable: 1;*/
	
	interface->interrupt_enable = (1 << 31) | (1 << 6);//(1 << 31) | (1 << 1) | (1 << 6);//0xFFFFFFFF;//0x80000042;
	interface->interrupt_disable = 0x00000000;
}


//////////////////////////////////////////////////////////////////


typedef struct {
	Number8  size;
	Number8  reserved;
	Number16 version;
	/*struct {
		Number32 number_of_ports:4;
		Number32 port_power:4; // 0 - auto
		Number32 number_of_companion_ports:4;
		Number32 companion_controller_number:4;
		Number32 port_indicators:16;
	}*/
	Number32 structural_parameters;
	/*struct {
		Number32 compatible_with_64:1;
		Number32 enable_periodic_queue:1;
		Number32 enable_async_queue:1;
		Number32 reserved1:1;
		Number32 isochronus_scheduling_threshold:4;
		Number32 extended_capabilities_pointer:8;
	}*/
	Number32 capability_parameters;
	Number32 companion_port_route;
}
EHCI_Capability;


typedef struct {
	/*struct {
		Number32 run:1;
		Number32 reset:1;
		Number32 frame_list_size:2; // 0 - 1024, 1 - 512, 2 - 256
		Number32 enable_periodic_schedule:1;
		Number32 enable_async_schedule:1;
		Number32 interrupt_on_async_advance_doorbell:1;
		Number32 light_reset:1;
		Number32 async_schedule_park_mode_count:2;
		Number32 zero1:1;
		Number32 async_schedule_park_mode_enable:1;
		Number32 zero2:4;
		Number32 number_of_microframes_to_process_between_interrupt:8;
		Number32 zero3:8;
	}*/
	Number32 command;
	/*struct {
		Number32 transfer_interrupt:1;
		Number32 error_interrupt:1;
		Number32 port_change_interrupt:1;
		Number32 frame_list_rellover:1;
		Number32 host_system_error:1;
		Number32 doorbell_interrupt:1;
		Number32 port_change:1;
		Number32 zero1:6;
		Number32 halted:1;
		Number32 reclamation:1;
		Number32 periodic_schedule_status:1;
		Number32 async_schedule_status:1;
		Number32 zero2:15;
	}*/
	Number32 status;
	/*struct {
		Number32 transfer_interrupt_enable : 1;
		Number32 error_interrupt_enable : 1;
	}*/
	Number32 interrupt;
	
	Number32 frame_index;
	Number32 segment_selector;
	Number32 frame_list_address;
	Number32 asynchronus_list_address;
	Number32 pad[10];
	Number32 config;
	
	/*struct {
		Number32 connected:1;
		Number32 connect_changed:1;
		Number32 enabled:1;
		Number32 enabled_changed:1;
		Number32 overcurrent:1;
		Number32 overcurrent_changed:1;
		Number32 force_resume:1;
		Number32 suspend:1;
		Number32 reset:1;
		Number32 zero1:1;
		Number32 line_status:2;
		Number32 port_power:1;
		Number32 companion_port_control:1; // 0 - local, 1 - companion host controller
		Number32 indicator:1; // 0 - off, 1 - amber, 2 - green
		Number32 port_test:4;
		Number32 wake_on_connect:1;
		Number32 wake_on_disconnect:1;
		Number32 wake_on_overcurrent:1;
		Number32 zero2:9;
	}*/
	Number32 port_status_and_control[32];
}
EHCI_Operation;


EHCI_Capability* capability;
EHCI_Operation*  operation;


void setup_EHCI_ports()
{
		Number i;
		Number number_of_ports;
		
		number_of_ports = capability->structural_parameters & 0b1111;
		
		for(i = 0; i < number_of_ports; ++i) {
			Number32 port_status = operation->port_status_and_control[i];
			
			if(!(port_status & 1)) {
				continue;
			}
			
			if(!port_status & (1 << 13)) {
				//operation->port_status_and_control[i] |= (1 << 2); //enabled
				//operation->port_status_and_control[i] |= (1 << 12);//port_power
				
				operation->port_status_and_control[i] |= (1 << 8); //reset
				//sleep(100);
				Number j;
				for(j = 0; j < 1000000000; ++j);
				
				port_status = operation->port_status_and_control[i];
			}
			
			print("port %d", i);
			
			if(port_status & 1) {
				print(" connected");
			}
			
			if(port_status & (1 << 1)) {
				print(" connect_changed");
			}
			
			if(port_status & (1 << 2)) {
				print(" enabled");
			}
			
			if(port_status & (1 << 12)) {
				print(" powered");
			}
			
			if(port_status & (1 << 13)) {
				print(" is_companion");
			}
			
			print("\n");
			
			//operation->port_status_and_control[i] |= (1 << 2); //enabled
			//operation->port_status_and_control[i] |= (1 << 12);//port_power
			
			//operation->port_status_and_control[i] &= ~(1 << 12);//port_power
			//operation->port_status_and_control[i] |= (1 << 12);//port_power
			/*
			operation->port_status_and_control[i] |= (1 << 8); //reset
			//sleep(100);
			Number j;
			for(j = 0; j < 1000000000; ++j);
			operation->port_status_and_control[i] |= (1 << 2); //enabled
			operation->port_status_and_control[i] |= (1 << 12);//port_power*/
		}
}


asm("pusha");
asm("call EHCI_interrupt_handler");
asm("mov $0x20, %al");
asm("out %al, $0x20");
asm("popa");
asm("iret");
void EHCI_interrupt_handler()
{
	print("EHCI interrupt\n");
	
	if(operation->status & (1 << 0)) {
		print("transfer\n");
	}
	if(operation->status & (1 << 1)) {
		print("error\n");
	}
	if(operation->status & (1 << 2)) {
		print("port_change_detect\n");
	}
	if(operation->status & (1 << 4)) {
		print("host_system_error\n");
	}
	if(operation->status & (1 << 5)) {
		print("doorbell_interrupt\n");
	}
	
	if(operation->status & (1 << 2)) { //port_change_detect
		setup_EHCI_ports();
	}
	
	print("frame index: %d\n", operation->frame_index >> 3);
}


void configure_USB_EHCI(Number16 address, PCI_Config* config)
{
	capability = get_PCI_MMIO_address(config, 0);
	operation = (Byte*)capability + capability->size;
	
	//disbale legacy
	Byte extended_capabilities_pointer;
	extended_capabilities_pointer = (capability->capability_parameters >> 8) & 0xFF;
	if(extended_capabilities_pointer >= 0x40) {
		//print("legacy enabled\n");
		Number32 legacy_support;
		legacy_support = *(Number32*)((Byte*)config + extended_capabilities_pointer);
		
		if(legacy_support & 0x00010000) { //HC BIOS Owned semaphore
			legacy_support |= 0x01000000; //HC OS Owned semaphore
			write_4bytes_in_PCI_config(address, extended_capabilities_pointer, legacy_support);
			
			do {
				legacy_support = read_4bytes_from_PCI_config(address, extended_capabilities_pointer);
			}
			while(!(legacy_support & 0x01000000) && (legacy_support & 0x00010000));
			
			print("legacy disabled\n");
		}
	}

	/*
	//operation->command &= ~(1); //stop
	
	operation->status = 0x3f; //clear status
	
	operation->command |= (8 << 16) | (1 << 1) | (1 << 4) | (1 << 5); //? | reset | periodic_schedule_enable | async_schedule_enable
	while(operation->status & (1 << 12)); //halted

	print("done");
	//print("%d\n", operation->command);*/
	
	if(config->interrupt_line >= 0 && config->interrupt_line < 16) {
		loader_api->set_interrupt_handler((Number)&EHCI_interrupt_handler - 12, config->interrupt_line + 32);
	}
	else {
		print("no interrupts, interrupt_line is %d\n", config->interrupt_line);
	}
	
	operation->interrupt |= 1 | (1 << 1) | (1 << 2) | (1 << 5); //transfer_interrupt_enable | error_interrupt_enable | port_change_interrupt_enable | async_advance_interrupt_enable
	//operation->command |= 1; //run
	
	//setup_EHCI_ports();
}


void configure_USB_devices()
{
	PCI_Config config;
	Number     i;
	
	for(i = 0; i < 65536; ++i) {
		if(read_PCI_config(i, &config)) {
			//print("found ven_%x%x dev_%x%x\n", config.vendor >> 8, config.vendor, config.device >> 8, config.device);
			
			if(config.class == 12 && config.subclass == 3 && config.prog_IF == 0) {
				//print("found USB UHCI\n");
				//configure_USB_UHCI(i, &config);
			}
			else if(config.class == 12 && config.subclass == 3 && config.prog_IF == 16) {
				//print("found USB OHCI\n");
				//configure_USB_OHCI(i, &config);
			}
			else if(config.class == 12 && config.subclass == 3 && config.prog_IF == 32) {
				print("found USB EHCI\n");
				configure_USB_EHCI(i, &config);
			}
			else if(config.class == 12 && config.subclass == 3 && config.prog_IF == 48) {
				//print("found USB XHCI\n");
			}
		}
	}
}


#endif//USB_INCLUDED