@mkdir "bin/components"
@mkdir "bin/components/devices"

@nasm "components/FAT32.asm" -o "bin/components/FAT32" ^
	&& nasm "components/MBR.asm" -o "bin/components/MBR" ^
	&& nasm "components/kernel/main16.asm" -o "bin/components/kernel16" ^
	&& tcc32 -w "-Icomponents/global" -nostdlib -c "components/kernel/main32.c" -o "bin/components/kernel32.elf" ^
	&& load "bin/components/kernel16" | to ld > "bin/components/kernel16.ld" ^
	&& ld -T kernel_script.ld -o "bin/components/kernel32.o" "bin/components/kernel32.elf" ^
	&& objcopy -O binary -S "bin/components/kernel32.o" "bin/components/kernel" ^
	&& build_device.bat "ps2-keyboard" ^
	&& build_device.bat "ps2-mouse" ^
	&& build_device.bat "PCI" ^
	&& build_device.bat "USB" ^
	&& build_COM_program.bat "shell" ^
	&& build_COM_program.bat "shell2" ^
	&& build_COM_program.bat "open" ^
	&& build_COM_program.bat "load" ^
	&& build_COM_program.bat "store" ^
	&& build_COM_program.bat "reset" ^
	&& build_COM_program.bat "video" ^
	&& build_EXE_program.bat "test" ^
	&& create MBR 2097664 "bin/components/MBR" > "bin/storage" && create FAT32 "bin/storage" "bin/components/FAT32" ^
		"bin/components/devices/ps2-keyboard" "DEVICES/KEYBOARDCOM" ^
		"bin/components/devices/ps2-mouse" "DEVICES/MOUSE   COM" ^
		"bin/components/devices/PCI" "DEVICES/PCI     COM" ^
		"bin/components/devices/USB" "DEVICES/USB     COM" ^
		"bin/components/kernel" "KERNEL" ^
		"bin/components/shell" "SHELL   COM" ^
		"bin/components/shell2" "SHELL2  COM" ^
		"bin/components/test.exe" "TEST    EXE" ^
		"components/programs/NES/NES.exe" "NES/NES     EXE" ^
		"bin/components/open" "OPEN    COM" ^
		"bin/components/load" "LOAD    COM" ^
		"bin/components/store" "STORE   COM" ^
		"bin/components/reset" "RESET   COM" ^
		"bin/components/video" "VIDEO   COM" ^
	&& "qemu\qemu.exe" -usb -m 256 -no-reboot -hda "bin\storage" ^
	|| pause