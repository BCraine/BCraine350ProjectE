bcc -ansi -c -o kernel_c.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel_c.o kernel_asm.o
./loadFile kernel
dd if=message.txt of=diskc.img bs=512 count=1 seek=30 conv=notrunc
./loadFile message.txt
bcc -ansi -c -o shell_c.o shell.c
as86 userlib.asm -o userlib.o
ld86 -o shell -d shell_c.o userlib.o
./loadFile shell
