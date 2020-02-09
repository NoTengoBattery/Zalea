# The Zalea microkernel

This is the Zalea microkernel. This is a small toy microkernel designed to be
portable, simple and fast. It uses advanced instructions, it implements a
"device tree blob", and also can be built with several compiler in Linux,
Windows and macOS.

It's build system is designed to be portable, as such, the only sane tool to do
the job is CMake. Along with CMake, a couple of Python scripts are provided to
extend the language.

This allows advanced features, such as generating the Hashmap from a JSON
properties file (which is the DTB) and allows to generate the `config.h` file
and the equivalent to Linux's "defconfig".

This project is built from scratch, almost all code is original and it's licence
is the Apache 2.0 Licence. Currently, this project is only developed by:

> Oever González <notengobattery@gmail.com>

## Dependencies

In order to build this project, you will need:

- CMake (3.15) and an adequate build tool
  - for macOS you need Xcode (with AppleClang 11 or better)
  - for Windows you can use Ninja, but MinGW is probably a better option
  - for Linux, please install your distro's development tools
- Python 3 (3.7)
  - with an active connection to the internet, because the build system will
    install a custom Python Virtual Environment with all the needed packages
- A suitable compiler for building the project
  - you can use the Clang compiler version 9.0.0 or better
  - or the GNU Compiler Collection version 9.0 or better
  - or Intel's ICC compiler version 19.0 or better
- Binary utilities to perform the final image generation
  - `objcopy` is needed, but can be found in either
    - LLVM version 9.0.0 or better
    - GNU Binutils version 2.33 or better
    - alternatively, you can use the GNU Gold linker

Some extra features that might be useful:

- ccache
- clang-tidy
- clangd
- CLion IDE
- Code::Blocks
- cpplint
- cppcheck
- Doxygen
- GNU Debugger or LLVM's LLDB
- GNU GRUB bootloader (for x86)
- openocd (Open On-Chip Debugger)
- qemu system

# Supported targets

The supported targets are available under the Architecture subdirectory. This is
a big work in progress, but the currently supported targets are:

- Intel Pentium 4 with SSE2 (Legacy BIOS machines from 2002 and onwards)
- Raspberry Pi 2 Model B, the QEMU machine is fine
- ARM RealView Platform Explorer (Cortex-A9), the QEMU machine is fine
- ARM Versatile Express (Cortex-A15), the QEMU machine is fine
- MIPS Creator CI20 (MIPS32), you will need a physical device
