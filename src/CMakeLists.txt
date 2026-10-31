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
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        qvistools::warnings
)
