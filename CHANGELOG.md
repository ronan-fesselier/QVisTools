# Changelog

## [Unreleased]

### Added

#### Dependencies

- `Qt6::Widgets`
- `dnv::vista::sdk`, resolved via `find_package` falling back to `FetchContent`

- `doctest`, fetched when `QVIS_TOOLS_BUILD_TESTS` is enabled (development only)

#### Build

- `qvistools::version()`: runtime-queryable project version, generated from `CMakeLists.txt`'s `project(VERSION ...)`

#### Core

- `VIS` singleton initialization at startup, with Gmod resolved for the latest VIS version

- `qvistools::core::utils::naturalCompare()`: natural-order string comparison (numeric runs compared by value, not lexicographically), locale-independent and byte-order-correct (unsigned char comparison)

- `qvistools::core::vis::sortedChildren()`: display-ready Gmod node children, with transparent product type/selection substitution and natural code ordering

#### GUI

- `MainWindow`: main window with status bar (showing the Gmod VIS version), File menu and Help menu (About dialog listing application, Qt and `dnv-vista-sdk` versions)

### Changed

- NIL

### Deprecated

- NIL

### Removed

- NIL

### Fixed

- NIL

### Security

- NIL
