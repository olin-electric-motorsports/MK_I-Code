TARGET=$(FILE)

LD_LIBRARY_PATH=lib
CC=avr-gcc
MCU=atmega16m1
PROGRAMMER=avrispmkII
PORT=usb
AVRDUDE=avrdude
OBJCOPY=avr-objcopy


##############
#  Src Vars  #
##############
SRCDIR=src
LIBDIR=lib
OBJDIR=obj
BUIDIR=build

SRCLIBS=$(wildcard $(LIBDIR)/*.c)
INTLIBS=$(wildcard $(LIBDIR)/*.h)
OBJLIBS=$(patsubst $(LIBDIR)/%.c,$(LIBDIR)/%.o,$(SRCLIBS))

SRCS=$(wildcard $(SRCDIR)/$(TARGET)/*.c)
OBJS=$(patsubst $(SRCDIR)/$(TARGET)/%.c, $(OBJDIR)/%.o, $(SRCS))

################
#  Build Vars  #
################
CFLAGS+=-mmcu=$(MCU) -g -Os -Wall -Wunused -I$(INCDIR)/
LDFLAGS=-mmcu=$(MCU) -Wl,-Map=$(BUIDIR)/$(TARGET).map -lm
AVRFLAGS=-p $(MCU) -v -c $(PROGRAMMER) -P $(PORT)

#############
#  Recipes  #
#############

all: $(OBJLIBS) $(BUIDIR)/$(TARGET).elf $(BUIDIR)/$(TARGET).hex $(BUIDIR)/$(TARGET).srec

$(LIBDIR)/%.o: $(LIBDIR)/%.c $(LIBDIR)/%.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/$(TARGET)/%.c 
	$(CC) -c -o $@ $< $(CFLAGS) -L$(LIBDIR)

$(BUIDIR)/%.elf: $(OBJS) $(OBJLIBS)
	$(CC) $^ $(LDFLAGS) -o $@

$(BUIDIR)/%.srec: $(BUIDIR)/%.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

$(BUIDIR)/%.hex: $(OUTDIR)/%.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@


$(OBJS): | $(OBJDIR)
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(BUIDIR)

.PHONY: clean
clean: 
	rm -rf $(BUIDIR) $(OBJDIR) $(OBJLIBS)
