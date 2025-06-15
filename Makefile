MODULE = syshook

obj-m :=$(MODULE).o
$(MODULE)-objs += mod.o
$(MODULE)-objs += pgtable.o
$(MODULE)-objs += util.o
$(MODULE)-objs += syscall.o

# 内核源码目录
KDIR ?= /home/qiu/Android/Android_kernel_sourse/Non-GKI/mipad5

KDIR_out ?= $(KDIR)/out

# 当前目录
PWD := $(shell pwd)

# 交叉编译工具链设置
TOOLCHAIN_DIR := /home/qiu/Android/arm-gnu-toolchain-aarch64-none-elf
CROSS_COMPILE := $(TOOLCHAIN_DIR)/bin/aarch64-none-elf-
export CROSS_COMPILE

# 将工具链添加到PATH
export PATH := $(TOOLCHAIN_DIR)/bin:$(PATH)

# 编译工具
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump
STRIP := $(CROSS_COMPILE)strip
READELF := $(CROSS_COMPILE)readelf

EXTRA_CFLAGS += -O2                # 优化等级2
EXTRA_CFLAGS += -fvisibility=hidden  # 隐藏符号
EXTRA_CFLAGS += -fomit-frame-pointer # 省略帧指针
# EXTRA_CFLAGS += -g0 去除调试信息
EXTRA_CFLAGS += -fno-pic -fvisibility=hidden
EXTRA_CFLAGS += -fno-stack-protector
EXTRA_CFLAGS += -Wno-format 

# 架构设置
ARCH := arm64
export ARCH

# # 默认目标
.PHONY: all
all: modules

.PHONY: modules
modules:
	@echo "正在编译ARM64内核模块..."
	$(MAKE) -C $(KDIR) O=$(KDIR_out) M=$(PWD) modules
	@echo "编译完成!"
	@echo "生成的模块:"
	@ls -l *.ko
	@rm -rf *.o *.mod.c *.mod.o *.symvers *.order *.cmd  .*.o.cmd .tmp_versions

# # 清理
.PHONY: clean
clean:
	@echo "正在清理编译文件..."
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf *.o *.ko *.mod.c *.mod.o *.symvers *.order *.cmd *.o.cmd .tmp_versions
	rm -f *.ur-safe
	@echo "清理完成!"