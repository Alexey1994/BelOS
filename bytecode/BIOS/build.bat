@mkdir "bin/components"

@nasm "components/FS32.asm" -o "bin/components/FS32" ^
	&& cd "components/kernel" && compile_086 "main.086" > "../../bin/components/kernel.asm" && cd "../../" && nasm "bin/components/kernel.asm" -o "bin/components/kernel" ^
	&& cd "components/shell" && compile_lv2 "shell.lv2" | compile_lv1 > "../../bin/components/shell.lv0" && cd "../.." ^
	&& cd "components/test_program" && compile_lv2 "test.lv2" | compile_lv1 > "../../bin/components/test.lv0" && cd "../.." ^
	&& cd "components/eat" && compile_lv2 "eat.lv2" | compile_lv1 > "../../bin/components/eat.lv0" && cd "../.." ^
	&& create 2097152 > "bin/storage" && create FS32 "bin/storage" "bin/components/FS32" ^
		"bin/components/kernel" "kernel" ^
		"bin/components/shell.lv0" "shell" ^
		"bin/components/test.lv0" "test" ^
		"bin/components/eat.lv0" "eat" ^
	&& "qemu\qemu.exe" -m 4 -no-reboot -hda "bin\storage" ^
	|| pause