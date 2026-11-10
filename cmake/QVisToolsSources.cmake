#==============================================================================
# QVisTools - CMake Sources
#==============================================================================

set(QVIS_TOOLS_SOURCES)
list(APPEND QVIS_TOOLS_SOURCES
    ${QVIS_TOOLS_SOURCE_DIR}/app/App.cpp

    ${QVIS_TOOLS_SOURCE_DIR}/core/GmodPathUtils.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/core/GmodSearchIndex.cpp

    ${QVIS_TOOLS_SOURCE_DIR}/gui/GmodTree/GmodChipRenderer.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/gui/GmodTree/GmodSearchBar.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/gui/GmodTree/GmodSearchResultDelegate.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/gui/GmodTree/GmodTreeItemDelegate.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/gui/GmodTree/GmodTreeModel.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/gui/GmodTree/GmodTreeView.cpp
    ${QVIS_TOOLS_SOURCE_DIR}/gui/MainWindow/MainWindow.cpp

    ${QVIS_TOOLS_SOURCE_DIR}/main.cpp

    ${QVIS_TOOLS_DIR}/resources/icons/icons.qrc
)
