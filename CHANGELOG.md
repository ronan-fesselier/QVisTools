# Changelog

## [Unreleased]

### Added

#### Dependencies

- `Qt6::Widgets`
- `dnv::vista::sdk`, resolved via `find_package` falling back to `FetchContent`

#### Build

- `qvistools::version()`: runtime-queryable project version, generated from `CMakeLists.txt`'s `project(VERSION ...)`

#### Core

- `VIS` singleton initialization at startup, with Gmod resolved for the latest VIS version

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
