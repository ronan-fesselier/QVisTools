# Changelog

## [Unreleased]

### Added

- GitHub Actions `Build` workflow: Linux (GCC, Clang) and Windows (MSVC) matrix build with strict warnings
- GitHub Actions `CodeQL` workflow: C/C++ static analysis

- `Qt6 Widgets` dependency

- `VIS` singleton initialized at startup

- `qvistools::version()`: runtime-queryable project version, generated from `CMakeLists.txt`'s `project(VERSION ...)`

- `App`: application entry point wrapping `QApplication` lifecycle

- `MainWindow`: main window with status bar, File menu and Help menu

- `GmodExplorerWidget`: top-level Gmod explorer - search bar, collapse/version/legend controls, a `QSplitter` with the tree on the left and a node detail panel on the right
- `GmodTreeWidget`: owns the Gmod tree view, model and item delegate; `currentNode()`/`currentPath()`, `navigateTo()`, `nodeSelectionChanged` signal
- `GmodNodeDetailWidget`: read-only panel for the selected node's code/name/category/definition and short/full Gmod path, with "Copy" buttons
- `GmodTreeModel`: lazy `QAbstractItemModel` over the Gmod DAG, naturally sorted, duplicate-safe expand/collapse state for nodes with multiple parents, `Qt::ToolTipRole` showing node definition
- `GmodTreeItemDelegate`: custom `QStyledItemDelegate` rendering node code, product and skipped-parent chips with category/type-based colors
- `GmodSearchIndex`: Qt-agnostic search index over the Gmod DAG, matching on code/name/commonName/definition
- `GmodSearchBar`: search box and results list above the tree view, with Escape/arrow/Enter keyboard navigation
- `GmodSearchResultDelegate`: renders search results with the same ancestor-path chip style as the tree
- `GmodChipRenderer`: shared chip-drawing logic. `Category` enum (mirroring DNV's Gmod principles: function group/selection, product type/selection) with `categoryOf()`/`categoryColor()`/`categoryName()`
- `GmodPathUtils`: `ancestorPath()` / `shortAncestorPath()` helpers for walking a node's ancestor chain
- `GmodTreeModel::indexForNode()`: locates a `GmodNode`'s model index by walking its ancestor chain, used to expand and select a search result in the tree
- `GmodTreeModel::pathForNode()`: builds a `GmodPath` for a node by walking its ancestor chain, used to show/copy its short and full path
- `core::buildLocalId()`: builds a `LocalId` string from a primary/secondary `GmodPath`
- `LocalIdBuilderWidget`: panel for building a `LocalId` from the selected `GmodPath`

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
