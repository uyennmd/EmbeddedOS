#--------------------------------------Makefile------------------------------------- 
OBJ_DIR = object
SRC_DIR = source

CFILES = $(wildcard $(SRC_DIR)/*.c) 
OFILES = $(CFILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) 
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib  
 
all: clean $(OBJ_DIR)/kernel8.img #run
 
$(OBJ_DIR)/start.o: $(SRC_DIR)/start.S 
	aarch64-none-elf-gcc $(CFLAGS) -c $(SRC_DIR)/start.S -o $(OBJ_DIR)/start.o 
 
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	aarch64-none-elf-gcc $(CFLAGS) -c $< -o $@ 
 
$(OBJ_DIR)/kernel8.img: $(OBJ_DIR)/start.o $(OFILES) 
	aarch64-none-elf-ld -nostdlib $(OBJ_DIR)/start.o $(OFILES) -T ./script/link.ld -o $(OBJ_DIR)/kernel8.elf 
	aarch64-none-elf-objcopy -O binary $(OBJ_DIR)/kernel8.elf $(OBJ_DIR)/kernel8.img 
 
clean: 
	del .\$(OBJ_DIR)\kernel8.elf .\$(OBJ_DIR)\*.o .\$(OBJ_DIR)\*.img 

run:
	qemu-system-aarch64 -M raspi3 -kernel $(OBJ_DIR)/kernel8.img -serial null -serial stdio