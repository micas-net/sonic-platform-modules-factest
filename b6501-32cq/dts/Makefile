INCLUDE_DIR := $(shell pwd)/../../dts

DTS_FILES = b6501-32cq_v1.00.dts
DTB_FILES = $(patsubst %.dts, %.dtb, $(DTS_FILES))

all: $(DTB_FILES)

%.dtb: %.dts
	cpp -nostdinc -I$(KERNEL_SRC)/source/include -I$(INCLUDE_DIR) -undef -x assembler-with-cpp $< > tmp.dts
	dtc -I dts -O dtb -o $@ tmp.dts

clean:
	rm -f $(DTB_FILES)
