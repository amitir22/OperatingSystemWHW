#!/bin/bash

CLION_PATH='~/CLionProjects/OperatingSystemWHW/HW2'

echo '>> installing dos2unix'
sudo apt install -y dos2unix
echo '>> dos2unix installed.'

dos2unix '~/$(CLION_PATH)/arch/x86/entry/syscalls/syscall_64.tbl'
dos2unix '~/$(CLION_PATH)/include/linux/init_task.h'
dos2unix '~/$(CLION_PATH)/include/linux/sched.h'
dos2unix '~/$(CLION_PATH)/include/linux/syscalls.h'
dos2unix '~/$(CLION_PATH)/kernel/Makefile'
dos2unix '~/$(CLION_PATH)/kernel/hw2.c'

echo '>> finished unixing all source files'

cd ~/linux-4.15.18-custom/
pwd

echo ''
echo '>> copying source files from clion folder to custom kernel folder:'

cp '$(CLION_PATH)/arch/x86/entry/syscalls/syscall_64.tbl' arch/x86/entry/syscalls/syscall_64.tbl
cp '$(CLION_PATH)/include/linux/init_task.h' include/linux/init_task.h
cp '$(CLION_PATH)/include/linux/sched.h' include/linux/sched.h
cp '$(CLION_PATH)/include/linux/syscalls.h' include/linux/syscalls.h
cp '$(CLION_PATH)/kernel/Makefile' kernel/Makefile
cp '$(CLION_PATH)/kernel/hw2.c' kernel/hw2.c

echo '>> done.'
echo ''

echo '>> remaking the kernel source code:'
echo ''

echo '>> cleaning:'
make clean
echo '>> done.'
echo ''

echo '>> compiling kernel:'
make -j $(nproc)
echo '>> done.'
echo ''

echo '>> installing modules:'
sudo make modules_install
echo '>> done.'
echo ''

echo '>> installing new kernel:'
sudo make install
echo '>> done.'
echo ''

echo '>> remaking: done. now just execute: sudo reboot and pray.'
echo ''
