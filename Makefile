
INCLUDE_PATHS = -I.  
LINKER_SCRIPT = ./config/LPC1768.ld
PROJECT = main



CC      = $(GCC_BIN)arm-none-eabi-gcc
CPP     = $(GCC_BIN)arm-none-eabi-g++
LD      = $(GCC_BIN)arm-none-eabi-gcc
OBJCOPY = $(GCC_BIN)arm-none-eabi-objcopy
CCLOCAL = gcc

CPU = -mcpu=cortex-m3 -mthumb
CC_FLAGS = -D_USA_ARM_  $(CPU) -c -fno-common -fmessage-length=0 -Wall -fno-exceptions -ffunction-sections -fdata-sections -g 
CC_SYMBOLS = -DTARGET_LPC1769 -DTOOLCHAIN_GCC_ARM -DNDEBUG -D__CORTEX_M3
LD_FLAGS = -mcpu=cortex-m3 -mthumb -Wl,--gc-sections,-Map=$(PROJECT).map,--cref 
LD_SYS_LIBS = -lc -lgcc -lnosys

BASICOS    = src/system_LPC17xx.o src/startup_LPC17xx.o src/core_cm3.o
MEUPROJETO = main.o SA_baixo.o i2c_software.o digital.o delay.o uart.o memoria.o



$(PROJECT).bin: $(PROJECT).elf nxpsum
	# *************************** $(PROJECT).bin *****************************  
	$(OBJCOPY) -O binary $< $@
	./nxpsum $@
	# ****************************************************************


$(PROJECT).elf: $(BASICOS)  $(MEUPROJETO) 
	# *************************** $(PROJECT).elf *****************************  
	$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $(addprefix $(BUILD_DIR), $^) $(LIBRARIES) $(LD_SYS_LIBS) $(LIBRARIES) $(LD_SYS_LIBS)

	# ****************************************************************






main.o:	main.cpp
	# *************************** main *****************************
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -std=gnu++98 main.cpp -c
	# **************************************************************


SA_baixo.o: SA_baixo.cpp SA_baixo.h memoria.o memoria.h
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -std=gnu++98 SA_baixo.cpp -c

memoria.o:  memoria.h i2c_software.o memoria.cpp
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -std=gnu++98 memoria.cpp -c

i2c_software.o:	i2c_software.cpp i2c_software.h digital.h delay.o
	# *************************** main *****************************
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -std=gnu++98 i2c_software.cpp -c
	# **************************************************************

digital.o:	digital.cpp digital.h
	# *************************** main *****************************
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -std=gnu++98 digital.cpp -c
	# **************************************************************

uart.o:	uart.cpp uart.h
	# *************************** main *****************************
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -std=gnu++98 uart.cpp -c
	# **

delay.o:	delay.cpp delay.h
	# *************************** main *****************************
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -std=gnu++98 delay.cpp -c
	# **************************************************************



src/core_cm3.o:	src/core_cm3.c
	# *************************** core_cm3 *************************
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -std=gnu99 src/core_cm3.c -c  -o src/core_cm3.o
	# **************************************************************


src/system_LPC17xx.o:	src/system_LPC17xx.c
	# *********************** system_LPC17xx ***********************
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -std=gnu99 src/system_LPC17xx.c -c -o src/system_LPC17xx.o
	# **************************************************************


src/startup_LPC17xx.o:	src/startup_LPC17xx.c
	# *********************** startup_LPC17xx **********************
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -std=gnu99 src/startup_LPC17xx.c -c -o src/startup_LPC17xx.o
	# **************************************************************

nxpsum:
	# **************************** nxpsum ****************************
	$(CCLOCAL) nxpsum.c -std=c99 -o nxpsum
	# ****************************************************************
	
clean:
	rm -f *.elf *.bin *.o src/*.o *.map nxpsum

flash:
	 lpc21isp -control -bin $(PROJECT).bin /dev/ttyUSB0 115200 12000

