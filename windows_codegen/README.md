You don't need to ship any of this code to use ggfiber, it's used to generate the inline bytecode in
ggfiber.cpp because MSVC doesn't support inline assembly.

Usage:

- `fasm.exe FiberWrapper.fasm`
- `lua.exe bin_to_c.lua FiberWrapper.bin`
- Reformat by hand

