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
- `GmodTreeItemDelegate`: custom `QStyledItemDelegate` rendering node code, product and skipped-parent chips with category/type-based colors
- `GmodSearchIndex`: Qt-agnostic search index over the Gmod DAG, matching on code/name/commonName/definition
- `GmodSearchBar`: search box and results list above the tree view, with Escape/arrow/Enter keyboard navigation
- `GmodSearchResultDelegate`: renders search results with the same ancestor-path chip style as the tree
- `GmodChipRenderer`: shared chip-drawing logic
- `GmodPathUtils`: `ancestorPath()` / `shortAncestorPath()` helpers for walking a node's ancestor chain

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
