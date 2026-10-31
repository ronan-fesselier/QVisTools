# QVisTools

[![GitHub](https://img.shields.io/github/license/ronan-fesselier/QVisTools?style=flat-square)](LICENSE)

Qt desktop toolkit for DNV's Vessel Information Structure (VIS).

## Installation

### Prerequisites

- **CMake** 3.25 or later
- **C++20 compiler**
- **Qt6** (Widgets) 6.5 or later

### Build from Source

```bash
git clone https://github.com/ronan-fesselier/QVisTools.git
cd QVisTools

cmake -B build -S .
cmake --build build
```

The `dnv-vista-sdk` dependency is resolved via `find_package`, falling back to `FetchContent` if not found on the system.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Third-party dependencies licenses

- **[DNV Vista SDK](https://github.com/dnv-opensource/vista-sdk)**: C++ implementation of DNV Vista SDK for VIS and ISO 19848 standard (MIT License). Built from [ronan-fesselier/vista-sdk](https://github.com/ronan-fesselier/vista-sdk/tree/feature/cpp/cpp) (C++ fork)
- **[Qt6](https://www.qt.io)**: Widgets module (GNU Lesser General Public License v3) - linked dynamically
