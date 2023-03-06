.PHONY: all
all: test1.bin

#.PRECIOUS: %.elf %.o

%.bin: %.o
	riscv64-linux-gnu-objcopy -O binary $< $@

%.o: %.s
	riscv64-linux-gnu-as $< -o $@

.PHONY: clean
clean:
	-rm -f *.o *.elf *.bin

