SHELL = /bin/bash
.ONESHELL:
.SHELLFLAGS += -e

EPDM_PATH=./epdm_4_3_8
EPIL_PATH=./APT2_2_2

EPDM_TARGET = libepdm.a
EPIL_TARGET = libphymodepil.a

all:
	pushd $(EPDM_PATH);\
	make;\
	cp ./$(EPDM_TARGET) ../
	popd; \
	pushd $(EPIL_PATH);\
	make;\
	cp ./build/$(EPIL_TARGET) ../;\
	popd; \

clean:
	pushd $(EPDM_PATH);\
	make clean;\
	popd; \
	pushd $(EPIL_PATH);\
	make clean;\
	popd;
	rm $(EPDM_TARGET) $(EPIL_TARGET)