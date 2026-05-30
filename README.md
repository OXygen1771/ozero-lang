# OZero-Lang

A simple interpreted programming language for learning and research.
The interpreter is implemented in C23 for now.

## Status

This language is in *very* early development, like many a project of mine. It might not ever be finished! However, the planned features include:

- [ ] Basic interpreter infrastructure
- [ ] Simple expressions (e.g. variable declarations and arithmetic operations)
- [ ] Control flow expressions and loops 
- [ ] Simple user-defined functions
- [ ] Many(?) more to come...

## Quickstart

This project uses CMake as its meta-build system (unfortunately).

**To get started:**

1. Install a C compiler that supports `C23` (e.g. `gcc`, `clang`, `MSVC`...)

2. Install `cmake` (of version >= 3.20)

3. Build and run `ozero`

```bash
# note: there is no difference between release and debug yet...
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build && cd build/target

# run!
ozero <args>
```

## Credits

The project is inspired by and created while reading the amazing Robert Nystrom's book [Crafting Interpreters](https://craftinginterpreters.com/), check it out! The Rust language also was an inspiration for the project (mainly for the language syntax).

I do not claim that this is a completely original language, but I strive to do things on my own with but guidance from the book. After all, this is just a learning project.

## License

This project is licensed under the [MIT](./LICENSE) (Expat) license.
