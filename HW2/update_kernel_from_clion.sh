#!/bin/bash

echo '>> installing dos2unix'
sudo apt install -y dos2unix
echo '>> dos2unix installed.'

dos2unix ~/CLionProjects/OperatingSystemWHW/HW2/arch/x86/entry/syscalls/syscall_64.tbl
dos2unix ~/CLionProjects/OperatingSystemWHW/HW2/include/linux/init_task.h
dos2unix ~/CLionProjects/OperatingSystemWHW/HW2/include/linux/sched.h
dos2unix ~/CLionProjects/OperatingSystemWHW/HW2/include/linux/syscalls.h
dos2unix ~/CLionProjects/OperatingSystemWHW/HW2/kernel/Makefile
dos2unix ~/CLionProjects/OperatingSystemWHW/HW2/kernel/hw2.c

echo '>> finished unixing all source files'

cd ~/linux-4.15.18-custom/
pwd

echo ''
echo '>> copying source files from clion folder to custom kernel folder:'

cp ~/CLionProjects/OperatingSystemWHW/HW2/arch/x86/entry/syscalls/syscall_64.tbl ~/linux-4.15.18-custom/arch/x86/entry/syscalls/syscall_64.tbl
cp ~/CLionProjects/OperatingSystemWHW/HW2/include/linux/init_task.h ~/linux-4.15.18-custom/include/linux/init_task.h
cp ~/CLionProjects/OperatingSystemWHW/HW2/include/linux/sched.h ~/linux-4.15.18-custom/include/linux/sched.h
cp ~/CLionProjects/OperatingSystemWHW/HW2/include/linux/syscalls.h ~/linux-4.15.18-custom/include/linux/syscalls.h
cp ~/CLionProjects/OperatingSystemWHW/HW2/kernel/Makefile ~/linux-4.15.18-custom/kernel/Makefile
cp ~/CLionProjects/OperatingSystemWHW/HW2/kernel/hw2.c ~/linux-4.15.18-custom/kernel/hw2.c

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
