# os
os learning and tests
VSL=very simple linux build with Busybox
w_initramfs = linux with initramfs boot
wo_initramfs = linux without initramfs boot
1. compile kernel (make defconfig)
2. compile busybox (static)
3. create initramfs: copy all busybox files from _install to initramfs
4. create the init script in initramfs folder
5. then find . |cpio -o -H newc > initram

6. w/o initramfs: create a new file dd if=/dev/zero of=boot.dsk, mkfs.ext4 boot.dsk, sudo mount boot.dsk m/, sudo extlinux --install m/, sudo cp [all busybox files and init script] to m/, create a folder boot and copy kernel, sudo umount m
7. [optiomal] create a syslinux.cfg file for automatic booting
