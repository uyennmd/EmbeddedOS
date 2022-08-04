#--------------------------------------Makefile------------------------------------- 
uart1 = Q1
OBJ_DIR = object
SRC_DIR = source



CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib  
CFILESu1 = $(wildcard ../$(uart1)/$(SRC_DIR)/*.c) 
OFILESu1 = $(CFILESu1:../$(uart1)/$(SRC_DIR)/%.c=../$(uart1)/$(OBJ_DIR)/%.o)  

CFILES = $(wildcard $(SRC_DIR)/*.c) 
OFILES = $(CFILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) 

uart1:cleanu1 ../$(uart1)/$(OBJ_DIR)/kernel8.img #run

uart5:cleanu5 $(OBJ_DIR)/kernel8.img #run
 
all: cleanu1 ../$(uart1)/$(OBJ_DIR)/kernel8.img #run
 
../$(uart1)/$(OBJ_DIR)/start.o: ../$(uart1)/$(SRC_DIR)/start.S 
	aarch64-none-elf-gcc $(CFLAGS) -c ../$(uart1)/$(SRC_DIR)/start.S -o ../$(uart1)/$(OBJ_DIR)/start.o 
 
../$(uart1)/$(OBJ_DIR)/%.o: ../$(uart1)/$(SRC_DIR)/%.c 
	aarch64-none-elf-gcc $(CFLAGS) -c $< -o $@ 
 
../$(uart1)/$(OBJ_DIR)/kernel8.img: ../$(uart1)/$(OBJ_DIR)/start.o $(OFILESu1) 
	aarch64-none-elf-ld -nostdlib ../$(uart1)/$(OBJ_DIR)/start.o $(OFILESu1) -T ../$(uart1)/script/link.ld -o ../$(uart1)/$(OBJ_DIR)/kernel8.elf 
	aarch64-none-elf-objcopy -O binary ../$(uart1)/$(OBJ_DIR)/kernel8.elf ../$(uart1)/$(OBJ_DIR)/kernel8.img
 
cleanu1: 
	del ..\$(uart1)\$(OBJ_DIR)\kernel8.elf ..\$(uart1)\$(OBJ_DIR)\*.o ..\$(uart1)\$(OBJ_DIR)\*.img 

run:
	qemu-system-aarch64 -M raspi3 -kernel ../$(uart1)/$(OBJ_DIR)/kernel8.img -serial null -serial stdio
 
$(OBJ_DIR)/boot.o: $(SRC_DIR)/boot.S 
	aarch64-none-elf-gcc $(CFLAGS) -c $(SRC_DIR)/boot.S -o $(OBJ_DIR)/boot.o 
 
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	aarch64-none-elf-gcc $(CFLAGS) -c $< -o $@ 
 
$(OBJ_DIR)/kernel8.img: $(OBJ_DIR)/boot.o $(OFILES) 
	aarch64-none-elf-ld -nostdlib $(OBJ_DIR)/boot.o $(OFILES) -T ./script/link.ld -o $(OBJ_DIR)/kernel8.elf 
	aarch64-none-elf-objcopy -O binary $(OBJ_DIR)/kernel8.elf $(OBJ_DIR)/kernel8.img 

cleanu5: 
	del .\$(OBJ_DIR)\kernel8.elf .\$(OBJ_DIR)\*.o .\$(OBJ_DIR)\*.img 
