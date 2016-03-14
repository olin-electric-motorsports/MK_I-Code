TARGET=$(FILE)

CC=avr-gcc
MCU=atmega16m1
PROGRAMMER=avrispmkII
PORT=usb
AVRDUDE=avrdude
OBJCOPY=avr-objcopy

SRCDIR=src
OBJDIR=obj
OUTDIR=build
INCDIR=$(SRCDIR)/inc

CFLAGS+=-mmcu=$(MCU) -g -Os -Wall -Wunused -I$(INCDIR)/
LDFLAGS=-mmcu=$(MCU) -Wl,-Map=$(OUTDIR)/$(TARGET).map -lm
AVRFLAGS=-p $(MCU) -v -c $(PROGRAMMER) -P $(PORT)

INCS=$(wildcard $(INCDIR)/*.h)
SRCS=$(SRCDIR)/$(TARGET).c \
	 $(patsubst $(INCDIR)/%.h,$(SRCDIR)/%.c,$(INCS))
OBJS=$(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))\
	 $(patsubst $(INCDIR)/%.c,$(OBJDIR)/%.o,$(INCS))

#############
#  Recipes  #
#############

all: $(OUTDIR)/$(TARGET).elf $(OUTDIR)/$(TARGET).hex $(OUTDIR)/$(TARGET).srec

$(OUTDIR)/%.srec: $(OUTDIR)/%.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

$(OUTDIR)/%.elf: $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

$(OUTDIR)/%.hex: $(OUTDIR)/%.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(wildcard $(INCDIR)/*.h) Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJS): | $(OBJDIR)
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(OUTDIR)

flash: $(OUTDIR)/$(TARGET).hex
	$(AVRDUDE) $(AVRFLAGS) -U flash:w:$<

verify: $(OUTDIR)/$(TARGET).hex
	$(AVRDUDE) $(AVRFLAGS) -U flash:v:$<

clean:
	rm -rf $(OUTDIR) $(OBJDIR)
