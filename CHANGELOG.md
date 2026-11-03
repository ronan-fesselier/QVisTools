# Changelog

## [Unreleased]

### Added

- About dialog with version information
- Gmod (Generic Product Model) loaded at startup, with its VIS version shown in the status bar
- Gmod tree explorer, with a VIS version selector to switch between available Gmod versions
- Gmod tree nodes sorted in natural order (e.g. `400a` before `411a`)
- Gmod tree nodes rendered as color-coded chips by category, with a link icon on mappable nodes and the underlying product type/selection node shown when a node substitutes one
- Gmod search bar, matching nodes by code, name, or definition (including plural/singular word forms), with results shown as ancestor chip paths and jumping straight to the node in the tree
- Tooltips on chip hover (category name) and on tree/search rows (node definition)
- Legend button showing the category/color mapping used by the chips
- Local ID builder, with primary/secondary item slots and metadata tag rows (standard values) for the displayed codebooks. The Local ID is built live as fields change, with a verbose mode, a copy button, and build errors shown in a dedicated panel
- Mapping a Gmod tree node to the Local ID builder's primary/secondary slot from its context menu
- Location individualization dialog for mapped item slots, to set number, side, vertical, transverse, and longitudinal location values on individualizable path segments
- Universal ID builder alongside the Local ID builder, with an IMO number field and its own verbose mode
- Parsing an existing Local or Universal ID to fill the primary/secondary item slots, metadata tags, verbose mode, and IMO number, switching the Gmod explorer's VIS version if needed

#### Dependencies

- DNV Vista SDK
- Qt6 (Widgets)

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
