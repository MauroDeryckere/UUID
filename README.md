# UUID
[![CI](https://github.com/MauroDeryckere/UUID/actions/workflows/ci.yml/badge.svg)](https://github.com/MauroDeryckere/UUID/actions/workflows/ci.yml)

A small, lightweight UUID (Universally Unique Identifier) library for use in my C++ engine.

This library provides functionality for generating and handling UUIDs in a simple and portable way. It is designed to be minimal, cross-platform, and easily integrable into any C++ project.

## Features
Generate random UUIDs (version 4)

Parse and stringify UUIDs

Cross-platform support (Windows, Linux, macOS)

No external dependencies (except libuuid on Linux)

## Usage

```cpp
#include "uuid.h"

MauUUID::UUID id{ }
std::string str{ id.Str() };

auto parsed{ MauUUID::UUID::FromString(str) };
assert(parsed == id);

```

## Building
This library uses CMake for building. Example:

### Basic build (includes tests)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Build without tests
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DMAU_UUID_BUILD_TESTS=ON
cmake --build build
cd build
ctest --output-on-failure
```
Or set the option to OFF when adding the project in your cmake file to disable tests.
