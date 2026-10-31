# Changelog

## [Unreleased]

### Added

- `Qt6 Widgets` dependency

- `VIS` singleton initialized at startup

- `qvistools::version()`: runtime-queryable project version, generated from `CMakeLists.txt`'s `project(VERSION ...)`

- `App`: application entry point wrapping `QApplication` lifecycle

- `MainWindow`: main window with status bar, File menu and Help menu

- `GmodTreeView`: navigable Gmod tree panel
- `GmodTreeModel`: lazy `QAbstractItemModel` over the Gmod DAG, naturally sorted, duplicate-safe expand/collapse state for nodes with multiple parents

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
