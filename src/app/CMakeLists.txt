#==============================================================================
# QVisTools - app
#==============================================================================

add_executable(${PROJECT_NAME}
    ${QVIS_TOOLS_DIR}/resources/icons/icons.qrc

    main.cpp

    gui/GmodExplorer/GmodExplorer.cpp
    gui/GmodTree/GmodTreeItemDelegate.cpp
    gui/GmodTree/GmodTreeModel.cpp

    gui/MainWindow/MainWindow.cpp

    gui/Shared/GmodChipRenderer.cpp
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
        qvistools::core
        qvistools::warnings
)
