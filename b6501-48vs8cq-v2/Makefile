PWD		= $(shell pwd)
DIR_DTS_SRC = $(PWD)/dts
EXTRA_CFLAGS:= -I$(M)/include
EXTRA_CFLAGS+= -Wall
SUB_BUILD_DIR = $(PWD)/build
INSTALL_SCRIPT_DIR = $(SUB_BUILD_DIR)/usr/local/bin
INSTALL_LIB_DIR = $(SUB_BUILD_DIR)/usr/lib/python3/dist-packages
INSTALL_S3IP_SYSFS_CFG_DIR = $(SUB_BUILD_DIR)/etc/s3ip_sysfs_cfg
INSTALL_UPGRADE_TEST_DIR = $(SUB_BUILD_DIR)/etc/.upgrade_test
INSTALL_ETC_DIR = $(SUB_BUILD_DIR)/etc
INSTALL_DTB_DIR = $(SUB_BUILD_DIR)/boot

all: build install
	@echo "build b6501-48vs8cq-v2 success."

build:
	$(MAKE) -C $(DIR_DTS_SRC)

install:
	@if [ ! -d ${INSTALL_SCRIPT_DIR} ]; then mkdir -p ${INSTALL_SCRIPT_DIR} ;fi
	cp -r $(PWD)/config/*  $(INSTALL_SCRIPT_DIR)
	@if [ ! -d ${INSTALL_DTB_DIR} ]; then mkdir -p ${INSTALL_DTB_DIR} ;fi
	cp -r $(DIR_DTS_SRC)/*.dtb  $(INSTALL_DTB_DIR)
	@if [ ! -d ${INSTALL_LIB_DIR} ]; then mkdir -p ${INSTALL_LIB_DIR} ;fi
	@if [ -d $(PWD)/hal-config/ ]; then cp -r $(PWD)/hal-config/* ${INSTALL_LIB_DIR} ;fi
	@if [ ! -d ${INSTALL_ETC_DIR} ]; then mkdir -p ${INSTALL_ETC_DIR} ;fi
	@if [ -d $(PWD)/etc/ ]; then cp -r $(PWD)/etc/* ${INSTALL_ETC_DIR} ;fi
	@if [ ! -d ${INSTALL_S3IP_SYSFS_CFG_DIR} ]; then mkdir -p ${INSTALL_S3IP_SYSFS_CFG_DIR} ;fi
	@if [ -d $(PWD)/s3ip_sysfs_cfg/ ]; then cp -r $(PWD)/s3ip_sysfs_cfg/* ${INSTALL_S3IP_SYSFS_CFG_DIR} ;fi
	@if [ ! -d ${INSTALL_UPGRADE_TEST_DIR} ]; then mkdir -p ${INSTALL_UPGRADE_TEST_DIR} ;fi
	@if [ -d $(PWD)/.upgrade_test/ ]; then cp -r $(PWD)/.upgrade_test/* ${INSTALL_UPGRADE_TEST_DIR} ;fi
clean:
	rm -f ${DIR_KERNEL_SRC}/*.o ${DIR_KERNEL_SRC}/*.ko ${DIR_KERNEL_SRC}/*.mod.c ${DIR_KERNEL_SRC}/.*.cmd
	rm -f ${DIR_KERNEL_SRC}/Module.markers ${DIR_KERNEL_SRC}/Module.symvers ${DIR_KERNEL_SRC}/modules.order
	rm -rf ${DIR_KERNEL_SRC}/.tmp_versions
	rm -rf $(SUB_BUILD_DIR)
