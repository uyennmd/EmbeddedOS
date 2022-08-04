#--------------------------------------Makefile------------------------------------- 
CFILES=src
OFILES=build

SRCS = $(wildcard $(CFILES)/*.c) 
OBJS = $(SRCS:$(CFILES)/%.c=$(OFILES)/%.o) 
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib  
 
all: clean kernel8.img #run
 
$(OFILES)/boot.o: $(CFILES)/boot.S 
	aarch64-none-elf-gcc $(CFLAGS) -c $(CFILES)/boot.S -o $(OFILES)/boot.o 
 
$(OFILES)/%.o: $(CFILES)/%.c 
	aarch64-none-elf-gcc $(CFLAGS) -c $< -o $@ 
 
kernel8.img: $(OFILES)/boot.o $(OBJS) 
	aarch64-none-elf-ld -nostdlib $(OFILES)/boot.o $(OBJS) -T $(CFILES)/link.ld -o $(OFILES)/kernel8.elf 
	aarch64-none-elf-objcopy -O binary $(OFILES)/kernel8.elf kernel8.img 
 
clean: 
	del $(OFILES)\kernel8.elf $(OFILES)\*.o *.img 

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio

test: all run