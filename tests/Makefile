.PHONY: all
all: test_add.bin test_addi.bin

#.PRECIOUS: %.elf %.o

%.bin: %.o
	riscv64-linux-gnu-objcopy -O binary $< $@

%.o: %.s
	riscv64-linux-gnu-as $< -o $@

.PHONY: clean
clean:
	-rm -f *.o *.elf *.bin

