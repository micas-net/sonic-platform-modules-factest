SHELL = /bin/bash
# CC = gcc

BUILD_DIR = $(CURDIR)/build

OUTPUT_DIR = $(CURDIR)/output

BLDDIR = $(BUILD_DIR)
LOCALDIR = $(BUILD_DIR)
LIBDIR = $(BUILD_DIR)

VSRCS += $(wildcard ./src/*.c)
LSRCS = $(notdir $(VSRCS))
LOBJS = $(addsuffix .o,$(basename $(LSRCS)))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))

DEBUG  = -DUSE_DEBUG

CFLAGS=$(CFLAG)
CFLAGS+= -Wall -Wno-unused -fPIC

INCLUDES = -I ./include -I ./include/credo -I ./libs/epdm_4_3_8
LIB_PATH = ./libs

VLIBS = $(wildcard ./libs/*.a)
LLIBS = $(notdir $(VLIBS))
LLOBJS = $(addsuffix .o,$(basename $(LLIBS)))
LBOBJS += $(addprefix $(BLDDIR)/,$(LLOBJS))

TARGET_LIB = $(OUTPUT_DIR)/libsaphy.a
TARGET_BIN = $(OUTPUT_DIR)/saphy
TARGET_SO = $(OUTPUT_DIR)/libsaphy.so
TARGET_SOBIN = $(OUTPUT_DIR)/saphy_so

DEPENDENCY_LIB = ./libs/libblackhawk.a ./libs/libcrcli.a ./libs/libcredo.a ./libs/libepdm.a ./libs/libphymodepil.a

LIB_SRC = $(LIB_PATH)/

# .a
$(TARGET_LIB): $(BOBJS)
	@echo "Build libsaphy.a"
	@mkdir -p $(OUTPUT_DIR)
	ar -rcs $(TARGET_LIB) $^

# .bin
$(TARGET_BIN): $(BOBJS) dependency_libs
	@echo "Build saphy"
	@mkdir -p $(OUTPUT_DIR)
	$(CC) -o $@ $(BOBJS) -L $(LIB_PATH) -Wl,--whole-archive $(DEPENDENCY_LIB) -Wl,--no-whole-archive -lstdc++ -lpthread -ldl -lm

# .o
$(BOBJS): $(BLDDIR)/%.o: ./src/%.c ./include/cust_plp_common.h ./include/cust_plp_if_api.h ./include/cust_plp_debug.h 
	@mkdir -p $(BUILD_DIR)
	$(CC) $(INCLUDES) $(DEBUG) -c $(CFLAGS) $< -o $@ -DPLP_APERTA2_SUPPORT -DPLP_MACSEC_SUPPORT -DAPERTA2_FRC_SERBOOT_LOW

# .so
$(TARGET_SO): $(VSRCS) dependency_libs
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OUTPUT_DIR)
	$(CC) $(VSRCS) $(INCLUDES) $(DEBUG) $(CFLAGS) -shared -o $(TARGET_SO) -DPLP_APERTA2_SUPPORT -DPLP_MACSEC_SUPPORT -L $(LIB_PATH) -Wl,--whole-archive $(DEPENDENCY_LIB) -Wl,--no-whole-archive -lstdc++ -lpthread -ldl -lm -lpthread -lrt

# .so.bin
$(TARGET_SOBIN): ./src/cust_plp_debug.c
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OUTPUT_DIR)
	$(CC) ./src/cust_plp_debug.c $(INCLUDES) $(DEBUG) $(CFLAGS) -L $(OUTPUT_DIR) -lsaphy -o $@

all: bin install

install:
	@mkdir -p $(common_out_put_dir)
	cp -p $(OUTPUT_DIR)/* $(common_out_put_dir)	

dependency_libs:
	pushd $(LIB_SRC);\
	make all;\
	popd;

lib: $(TARGET_LIB) dependency_libs
	cp ./libs/*.a $(OUTPUT_DIR)

solib: $(TARGET_SO)

sobin: CFLAGS += -DEXTPHY_BINARY
sobin: solib $(TARGET_SOBIN)
	
bin: CFLAGS += -DEXTPHY_BINARY
bin: $(TARGET_BIN)

clean:
	@echo "Remove linked and compiled files......"
	-rm -rf $(BUILD_DIR) $(OUTPUT_DIR)
	pushd $(LIB_SRC);\
	make clean;\
	popd;

rebuild: clean all

.PHONY:all rebuild clean lib bin