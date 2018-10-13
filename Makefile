#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export TARGET	:=	$(shell basename $(CURDIR))
export TOPDIR	:=	$(CURDIR)

# specify a directory which contains the nitro filesystem
# this is relative to the Makefile
NITRO_FILES	:=

# Version information
export VERSION_MAJOR	:= 1
export VERSION_MINOR	:= 2
export VERSTRING	:=	$(VERSION_MAJOR).$(VERSION_MINOR)

# These set the information text in the nds file
GAME_ICON      := $(CURDIR)/icon.bmp
GAME_TITLE     := Tuna-viDS v$(VERSTRING)
GAME_SUBTITLE1 := AVI (Xvid+MP3) player
GAME_SUBTITLE2 := Created by Chishm

.PHONY: $(TARGET).arm7 $(TARGET).arm9

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: checkarm7 checkarm9 $(TARGET).nds

#---------------------------------------------------------------------------------
checkarm7	:	arm7/$(TARGET).elf

#---------------------------------------------------------------------------------
checkarm9	:	arm9/$(TARGET).elf

#---------------------------------------------------------------------------------
$(TARGET).nds	: $(NITRO_FILES) arm7/$(TARGET).elf arm9/$(TARGET).elf
	ndstool	-c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf \
	-b $(GAME_ICON) "$(GAME_TITLE);$(GAME_SUBTITLE1);$(GAME_SUBTITLE2)" \
	$(_ADDFILES)

#---------------------------------------------------------------------------------
arm9/source/version.h	: Makefile
	@echo "#ifndef VERSION_H_" > $@
	@echo "#define VERSION_H_" >> $@
	@echo >> $@
	@echo '#define VERSION_STRING "'$(VERSION_MAJOR).$(VERSION_MINOR)'"' >> $@
	@echo >> $@
	@echo "#endif // VERSION_H_" >> $@

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
arm9/$(TARGET).elf	:	arm9/source/version.h
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
dist-bin	: $(TARGET).nds README.md LICENSE
	zip -X -9 $(TARGET)_v$(VERSTRING).zip $^

dist-src	:
	tar --exclude=*~ -cvjf $(TARGET)_src_v$(VERSTRING).tar.bz2 \
	--transform 's,^,$(TARGET)/,' \
	Makefile icon.bmp LICENSE README.md \
	arm7/Makefile arm7/source \
	arm9/Makefile arm9/source

dist	:	dist-bin dist-src

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9
