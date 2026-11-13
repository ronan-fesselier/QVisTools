#==============================================================================
# QVisTools - CMake Sources
#==============================================================================

set(QVIS_TOOLS_SOURCES)
list(APPEND QVIS_TOOLS_SOURCES
    ${QVIS_TOOLS_SOURCE_DIR}/app/App.cpp

    ${QVIS_TOOLS_SOURCE_DIR}/core/GmodPathUtils.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/core/GmodSearchIndex.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/core/LocalIdBuilderUtils.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/core/UniversalIdUtils.cpp

    ${QVIS_TOOLS_SOURCE_DIR}/gui/GmodExplorer/GmodExplorerWidget.cpp

    ${QVIS_TOOLS_SOURCE_DIR}/gui/GmodTree/GmodNodeDetailWidget.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/gui/GmodTree/GmodTreeItemDelegate.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/gui/GmodTree/GmodTreeModel.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/gui/GmodTree/GmodTreeWidget.cpp

    ${QVIS_TOOLS_SOURCE_DIR}/gui/LocalId/LocalIdBuilderWidget.cpp

    ${QVIS_TOOLS_SOURCE_DIR}/gui/MainWindow/MainWindow.cpp

    ${QVIS_TOOLS_SOURCE_DIR}/gui/Search/GmodSearchBar.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/gui/Search/GmodSearchResultDelegate.cpp

    ${QVIS_TOOLS_SOURCE_DIR}/gui/Shared/GmodChipRenderer.cpp

    ${QVIS_TOOLS_SOURCE_DIR}/gui/UniversalId/UniversalIdWidget.cpp

    ${QVIS_TOOLS_SOURCE_DIR}/main.cpp

    ${QVIS_TOOLS_DIR}/resources/icons/icons.qrc
)
