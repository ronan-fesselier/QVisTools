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
- `qvistools::core::utils::toLower()`: ASCII lowercasing helper
- `qvistools::core::utils::containsCaseInsensitive()`/`asciiEqualCaseInsensitive()`: ASCII case-insensitive substring/byte matching without allocating a lowercased copy
- `qvistools::core::utils::stemSimple()`: strips a trailing plural 's' from a lowercase word
- `qvistools::core::utils::splitWords()`: splits a string into alphanumeric words

- `qvistools::core::vis::ancestorPath()`: resolves the Gmod root-to-node path via the first-parent heuristic
- `qvistools::core::vis::shortAncestorPath()`: same, keeping only leaf nodes - used to render a node's displayed chip path
- `qvistools::core::vis::effectiveNode()`: resolves the node whose children should be displayed in place of a node's own (transparent product type/selection substitution)
- `qvistools::core::vis::sortedChildren()`: display-ready Gmod node children, with transparent product type/selection substitution and natural code ordering
- `qvistools::core::vis::categoryOf()`: classifies a Gmod node into a display category (root, leaf, product type, function composition, group, selection)
- `qvistools::core::vis::SearchIndex`: case-insensitive search over a Gmod's nodes (code, name, common name, definition), combining a trigram-accelerated substring match with a multi-word stemmed match (every word of the search term must match a whole stemmed word, so "cooling pump" matches an entry containing "cooling" and "pump"/"pumps" in any order), reporting each match's displayed parent

#### GUI

- `MainWindow`: main window with status bar (showing the Gmod VIS version and node count), File menu and Help menu (About dialog listing application, Qt and `dnv-vista-sdk` versions)

- `GmodTreeModel`: `QAbstractItemModel` exposing the Gmod tree, with sorted/substituted children caching, `ProductNodeRole`/`SkippedParentNodeRole` exposing substitution context to the view, and a tooltip showing the node's common/plain definition
- `GmodTreeModel::indexForMatch()`: resolves the model index displaying a `SearchIndex::Match`'s node, at a cost proportional to the path depth
- `GmodTreeItemDelegate`: renders Gmod nodes as category-colored chips (with skipped-parent and product-type/selection context) followed by the node's display name, showing the hovered chip's category name as a tooltip

- `GmodExplorer`: tree view over the Gmod, with a VIS version selector, a collapse-all action, a search bar (navigating to and selecting an activated search match in the tree), and an info button showing a category-color legend as a tooltip
- `GmodChipRenderer`: maps a node category to a display color, picks a readable (black/white) text color for it, paints a chip-row (selection/background, chip group, elided name) shared by `GmodTreeItemDelegate`/`GmodSearchResultDelegate`, and hit-tests a point against chip segments for tooltips
- `GmodSearchBar`: text search over the Gmod backed by `core::vis::SearchIndex`, showing matches (with a tooltip of the matched node's common/plain definition) in an inline results list
- `GmodSearchResultDelegate`: renders search results as a leaf-only ancestor chip path followed by the matched node's display name, showing the hovered chip's category name as a tooltip

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
