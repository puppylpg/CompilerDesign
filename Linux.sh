nasm -f elf Assembly.asm -o code.o -F dwarf -g
gcc -o code code.o -m32
./code
