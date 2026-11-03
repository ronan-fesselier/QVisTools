#==============================================================================
# QVisTools - QtApp
#==============================================================================

add_executable(${PROJECT_NAME}
    $<$<PLATFORM_ID:Windows>:${QVIS_TOOLS_DIR}/resources/icons/app.rc>
    ${QVIS_TOOLS_DIR}/resources/icons/icons.qrc

    main.cpp

    GmodExplorer/GmodExplorer.cpp
    GmodExplorer/GmodSearchBar.cpp
    GmodExplorer/GmodSearchResultDelegate.cpp
    GmodExplorer/GmodTreeItemDelegate.cpp
    GmodExplorer/GmodTreeModel.cpp

    Id/GmodPathIndividualizeDialog.cpp
    Id/GmodPathItemSlot.cpp
    Id/IdErrorsWidget.cpp
    Id/IdPanel.cpp
    Id/LocalIdBuilderWidget.cpp
    Id/MetadataTagRow.cpp
    Id/ParseIdWidget.cpp
    Id/UniversalIdBuilderWidget.cpp

    MainWindow/MainWindow.cpp

    shared/GmodChipRenderer.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${QVIS_TOOLS_SOURCE_DIR}
        ${QVIS_TOOLS_GENERATED_INCLUDE_DIR}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        DEBUG_POSTFIX "${QVIS_TOOLS_DEBUG_POSTFIX}"
        AUTOMOC ON
        AUTORCC ON
        WIN32_EXECUTABLE "$<NOT:$<CONFIG:Debug>>"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Widgets
        dnv::vista::sdk
        viscore
        qvistools::warnings
)
