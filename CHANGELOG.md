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

- `qvistools::core::vis::buildLocalId()`: builds a LocalId string from a primary item, an optional secondary item and a set of metadata tags, collecting every invalid tag before failing rather than stopping at the first one
- `qvistools::core::vis::applyLocationToSet()`: applies a `Location` to one of a `GmodPath`'s individualizable sets, returning the resulting path
- `qvistools::core::vis::DISPLAYED_CODEBOOKS`: the 8 codebooks shown to the user (Quantity, Content, Calculation, State, Command, Type, Position, Detail), in `vis/codebooks/CodebookValues.h`
- `qvistools::core::vis::standardValuesOf()`: sorted standard (non-custom) values for a codebook, backing a metadata field's autocomplete

#### GUI

- `MainWindow`: main window with status bar (showing the Gmod VIS version and node count), File menu and Help menu (About dialog listing application, Qt and `dnv-vista-sdk` versions)

- `GmodTreeModel`: `QAbstractItemModel` exposing the Gmod tree, with sorted/substituted children caching, `ProductNodeRole`/`SkippedParentNodeRole` exposing substitution context to the view, and a tooltip showing the node's common/plain definition
- `GmodTreeModel::indexForMatch()`: resolves the model index displaying a `SearchIndex::Match`'s node, at a cost proportional to the path depth
- `GmodTreeItemDelegate`: renders Gmod nodes as category-colored chips (with skipped-parent and product-type/selection context) followed by the node's display name, showing the hovered chip's category name as a tooltip

- `GmodExplorer`: tree view over the Gmod, with a VIS version selector, a collapse-all action, a search bar (navigating to and selecting an activated search match in the tree), and an info button showing a category-color legend as a tooltip
- `GmodChipRenderer`: maps a node category to a display color, picks a readable (black/white) text color for it, paints a chip-row (selection/background, chip group, elided name) shared by `GmodTreeItemDelegate`/`GmodSearchResultDelegate`, and hit-tests a point against chip segments for tooltips
- `GmodSearchBar`: text search over the Gmod backed by `core::vis::SearchIndex`, showing matches (with a tooltip of the matched node's common/plain definition) in an inline results list
- `GmodSearchResultDelegate`: renders search results as a leaf-only ancestor chip path followed by the matched node's display name, showing the hovered chip's category name as a tooltip
- `GmodPathItemSlot`: clickable slot for the Local ID path bar's primary/secondary item, showing a dashed "+Primary"/"+Secondary" placeholder when empty or the selected `GmodPath`'s short path as one category-colored chip per leaf segment when filled, with keyboard activation and hover/press feedback via `QAbstractButton`
- `LocalIdBuilderWidget`: the Local ID path bar ("/IMO/dnv-v2/vis-<version>/+Primary/sec/+Secondary/meta"), with an IMO number field, a VIS version label updatable without resetting the rest of the bar, and primary/secondary `GmodPathItemSlot`s requesting item selection from their owner on click

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
