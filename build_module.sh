#!/bin/bash

# === 配置区 ===
# GKI内核modules_prepare源码路径 
# bazel-bin/common/kernel_aarch64_modules_prepare/modules_prepare_outdir.tar.gz
KERNEL_DIR=~/Android/Android_kernel_sourse/GKI/modules_prepare_outdir/61 
CLANG_DIR=~/Android/Android_kernel_sourse/GKI/61/prebuilts/clang/host/linux-x86/clang-r487747c  # Clang 工具链路径
OUT_DIR=${KERNEL_DIR}/out                      # 内核编译输出目录
MODULE_SRC_DIR=${PWD}                          # 当前模块目录
JOBS=$(nproc)

# === 交叉工具链设置 ===
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-android-
export CLANG_TRIPLE=aarch64-linux-gnu-

export PATH=${CLANG_DIR}/bin:$PATH

export LLVM=1
export CC="clang"
export LD="ld.lld"
export AR="llvm-ar"
export NM="llvm-nm"
export OBJCOPY="llvm-objcopy"
export OBJDUMP="llvm-objdump"
export STRIP="llvm-strip"
export READELF="llvm-readelf"
export HOSTCC="clang"
export HOSTCXX="clang++"
export HOSTLD="ld.lld"



# === 编译模块 ===
echo "🔨 开始编译内核模块..."

make -C ${KERNEL_DIR} \
    M=${MODULE_SRC_DIR} \
    ARCH=arm64 \
    CC=clang \
    LLVM=1 \
    CLANG_TRIPLE=${CLANG_TRIPLE} \
    modules

if [ $? -eq 0 ]; then
    echo "✅ 模块编译成功: $(find . -name '*.ko')"
    rm *.o *.mod.c *.mod .* *.order *.symvers 
else
    echo "❌ 模块编译失败"
    exit 1
fi
