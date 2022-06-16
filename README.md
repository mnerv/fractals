# Fractals

Fractals explorer.

## Requirements

  - [CMake](https://cmake.org)

## Development

Clone repository with `--recursive` flag

```sh
git clone --recursive git@github.com:mnerv/fractals.git
```

Update the submodules.

```sh
git submodule update --init --recursive
```

Use `cmake` to generate the `Build System` script.

```sh
cmake -S . -Bbuild
```

```sh
cmake --build build -j4
```

