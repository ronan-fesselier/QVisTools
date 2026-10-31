# QVisTools

[![GitHub](https://img.shields.io/github/license/ronan-fesselier/QVisTools?style=flat-square)](LICENSE)

Qt desktop toolkit for implementing ISO 19848 and DNV's Vessel Information Structure (VIS).

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

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for a history of changes to this project.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Third-party dependencies licenses

- **[Qt6](https://www.qt.io)**: Widgets module (GNU Lesser General Public License v3) - linked dynamically
- **[Icons8](https://icons8.com)**: Icons used throughout the application
