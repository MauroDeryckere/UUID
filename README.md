# UUID
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
