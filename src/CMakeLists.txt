#==============================================================================
# QVisTools - src
#==============================================================================

#----------------------------------------------
# Sources
#----------------------------------------------

set(APP_SOURCES
    main.cpp
)

#----------------------------------------------
# Executable
#----------------------------------------------

add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        DEBUG_POSTFIX "${QVIS_TOOLS_DEBUG_POSTFIX}"
        AUTOMOC ON
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Widgets
        qvistools::warnings
)
