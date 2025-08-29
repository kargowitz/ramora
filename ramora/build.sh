cd init
make
cd ..
cd terminal
make
cd ../video && make && cd ../vltfs && make
mv init/initfs.o init/initos.o terminal/io.o video/video.o vltfs/ata.o vltfs/base.o ../

ld ld -m elf_i386 -o ram.bin -Ttext 0x1000 initos.o initfs.o video.o io.o ata.o base.o --oformat binary