# Installing from Source

The Arch build system only uses CMake &mdash; nothing more. Before you begin, make sure you have a working C++ environment and CMake installed.

## Dependencies

- CMake 3.25
- c++ std=17 or above

## Windows

### Install

```
powershell -ExecutionPolicy Bypass -File .\scripts\install\install.ps1
```

### Unitstall

```
powershell -ExecutionPolicy Bypass -File .\scripts\uninstall\uninstall.ps1
```

## Unix-like system

### Install
```
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$HOME/.arch"
cmake --build build --config Release
cmake --install build
```

### Unitstall

```
```
