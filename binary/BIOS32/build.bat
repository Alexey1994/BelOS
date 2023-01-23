@mkdir "bin/components"

@nasm "components/FAT32.asm" -o "bin/components/FAT32" ^
	&& nasm "components/MBR.asm" -o "bin/components/MBR" ^
	&& nasm "components/kernel/main16.asm" -o "bin/components/kernel16" ^
	&& tcc32 -w "-Icomponents/global" -nostdlib -c "components/kernel/main32.c" -o "bin/components/kernel32.elf" ^
	&& load "bin/components/kernel16" | to ld > "bin/components/kernel16.ld" ^
	&& ld -T kernel_script.ld -o "bin/components/kernel32.o" "bin/components/kernel32.elf" ^
	&& objcopy -O binary -S "bin/components/kernel32.o" "bin/components/kernel" ^
	&& build_program.bat "test" ^
	&& build_program.bat "ls" ^
	&& build_program.bat "cat" ^
	&& build_program.bat "reset" ^
	&& create MBR 2097664 "bin/components/MBR" > "bin/storage" && create FAT32 "bin/storage" "bin/components/FAT32" ^
		"bin/components/kernel" "KERNEL" ^
		"bin/components/test" "TEST    COM" ^
		"bin/components/ls" "LS      COM" ^
		"bin/components/cat" "CAT     COM" ^
		"bin/components/reset" "RESET   COM" ^
	&& "qemu\qemu.exe" -m 256 -no-reboot -hda "bin\storage" ^
	|| pause