# Thoanco Minimal OS Kernel

Đây là một kernel tối giản (minimal kernel) tự viết từ đầu để học các khái niệm cơ bản trong OS Development:
- Global Descriptor Table (GDT)
- Interrupt Descriptor Table (IDT)
- PIC remapping và xử lý ngắt
- VGA text driver và Keyboard driver
- Multitasking cơ bản (Task Manager)

---

## Cấu trúc thư mục

.
├── include/                 (header files)
│   ├── common/              (types.h, graphicscontext.h)
│   ├── drivers/             (driver.h, keyboard.h, vga.h)
│   ├── hardwarecommunication/ (interrupts.h, port.h)
│   ├── gdt.h, multitasking.h, syscalls.h
├── src/                     (mã nguồn chính)
│   ├── loader.s             (boot loader)
│   ├── gdt.cpp, multitasking.cpp, syscalls.cpp, kernel.cpp
│   ├── drivers/             (driver, keyboard, vga)
│   ├── hardwarecommunication/ (interrupts.cpp, interruptstubs.s, port.cpp)
├── obj/                     (file object)
├── linker.ld                (linker script ELF i386)
├── makefile                 (build script)
├── mykernel.bin             (kết quả build)
├── mykernel.iso             (nếu có tạo ISO)
├── README.md                (tài liệu này)

---

## Yêu cầu build

- GCC hỗ trợ `-m32` (hoặc cross-compiler i686-elf-gcc)
- NASM hoặc AS để assemble file .s
- LD hỗ trợ ELF i386
- QEMU hoặc Bochs để chạy thử kernel

---

## Cách build

make clean
make

Kết quả: file mykernel.bin (hoặc mykernel.iso nếu có script tạo ISO GRUB)

---

## Chạy thử kernel với QEMU

qemu-system-i386 -kernel mykernel.bin

Hoặc chạy file ISO (nếu bạn đã tạo)

qemu-system-i386 -cdrom mykernel.iso

---

## Các thành phần chính

- Loader (loader.s): entry ASM, thiết lập GDT, nhảy sang kernel C++
- GDT & IDT (gdt.cpp, interrupts.cpp, interruptstubs.s): thiết lập phân đoạn và xử lý ngắt
- Multitasking (multitasking.cpp, multitasking.h): quản lý TaskManager, context switch
- Drivers (drivers/): keyboard PS/2, VGA text

---



## Lưu ý Multitasking

Nếu kernel chạy đa nhiệm, InterruptManager cần nhận con trỏ TaskManager* để switch context.
Trong DoHandleInterrupt():
if (interrupt == hardwareInterruptOffset) {
    if (taskManager != 0)
        esp = taskManager->Schedule(esp);
}

---

## Gợi ý mở rộng

- Thêm paging, quản lý bộ nhớ
- Viết syscall (int 0x80)
- Viết driver chuột PS/2
- Viết file system ảo

---

## License

Dự án chỉ dành cho mục đích học tập và nghiên cứu cá nhân.

## https://github.com/thonguyenquang/operating_system
---

Chúc bạn tự build kernel thành công! 🚀
