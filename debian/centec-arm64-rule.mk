currentdir = $(shell pwd)
CUSTOMS_DIRS = $(currentdir)/common_custom/common_factest

MODULE_DIRS += b6501-48vs8cq
MODULE_DIRS += b6501-32cq
MODULE_DIRS += b6231-48xs8cq-v2
MODULE_DIRS += b6501-48vs8cq-v2

export CUSTOMS_DIRS MODULE_DIRS
