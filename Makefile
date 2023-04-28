RELEASE_DIR:=rust/target/powerpc-unknown-linux-gnu/release/
ELF := $(RELEASE_DIR)pie
BIN := $(RELEASE_DIR)pie.bin
LOADER := $(RELEASE_DIR)loader
LOADER_SRC := loader.c
OBJCOPY:=powerpc-linux-gnu-objcopy
CC:=powerpc-linux-gnu-gcc

.PHONY: default run strace debug gdb $(ELF)

default: $(BIN) $(LOADER)

.ONESHELL:
$(ELF):
	cd rust
	cargo build --release

$(BIN): $(ELF)
	$(OBJCOPY) \
		-O binary \
		-j .init \
		-j .text \
		-j .rodata \
		-j .got2 \
		$< \
		$@

$(LOADER): $(LOADER_SRC)
	$(CC) \
		-o $@ \
		$<

run: $(BIN) $(LOADER)
	qemu-ppc \
		-L /usr/powerpc-linux-gnu/ \
		-- \
		$(LOADER) \
		$(BIN)

strace: $(BIN) $(LOADER)
	qemu-ppc \
		-L /usr/powerpc-linux-gnu/ \
		-strace \
		-- \
		$(LOADER) \
		$(BIN)

debug: $(BIN) $(LOADER)
	qemu-ppc \
		-L /usr/powerpc-linux-gnu/ \
		-g 1234 \
		-- \
		$(LOADER) \
		$(BIN)

gdb:
	gdb-multiarch \
		-ex "target remote localhost:1234" \
		-ex "set architecture powerpc:MPC8XX" \
		-ex "file $(LOADER)" \
		-ex "add-symbol-file $(ELF) -readnow 0x80000000"
