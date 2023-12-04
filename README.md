# FrazzOS
This is a 64-bit monolithic kernel that utilizes [Limine bootloader](https://github.com/limine-bootloader/limine).

### Building
Building this project requires a cross-compiler for x64. A great resource for creating a cross-compiler for x64 can be found [here](https://wiki.osdev.org/GCC_Cross-Compiler).

```make all```: Compile the kernel and create an ISO image.
```make all-hdd```: Compile the kernel and generate a raw image that can be flashed onto hardware.
```make run``` Compile an ISO image and run it with ```qemu```.
```make run-hdd``` Compile a raw image and run it with ```qemu```.

### Features
- [X] Limine Barebones
- [X] Global Descriptor Table
- [X] Interrupt Descriptor Table
- [ ] SIMD
- [X] Physical Memory Manager
- [X] Virtual Memory Manager
- [X] Paging
- [X] Heap Memory Manager
- [X] ACPI
- [X] APIC
- [X] IOAPIC
- [X] LAPIC
- [X] HPET
- [ ] APIC-TIMER
- [ ] SMP
- [ ] Multitasking
- [ ] Syscalls
- [ ] Userspace


