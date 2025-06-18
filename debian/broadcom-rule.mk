currentdir = $(shell pwd)
CUSTOMS_DIRS = $(currentdir)/common_custom/common_factest

MODULE_DIRS += m2-w6940-128x1-fr4
MODULE_DIRS += m2-w6940-64oc
MODULE_DIRS += m2-w6931-64qc

export CUSTOMS_DIRS MODULE_DIRS
