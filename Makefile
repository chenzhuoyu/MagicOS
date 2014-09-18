PSW = \!cjboy2008\!
SUBDIRS = boot kernel

.PHONY: all clean run debug $(SUBDIRS)

default: all

debug: boot.img
	bochs -q -f bochsrc.bxrc

run: boot.img
	echo c | bochs -q -f bochsrc.bxrc

all: $(SUBDIRS)
	cp boot/boot.img ./
	echo $(PSW) | sudo -S mount -o loop boot.img /mnt -o fat=12
	echo $(PSW) | sudo -S cp boot/osldr /mnt
	echo $(PSW) | sudo -S cp kernel/oskernel /mnt
	echo $(PSW) | sudo -S umount /mnt

clean: $(SUBDIRS)
	rm -f boot.img

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
