#==============================================================================
# viscore
#==============================================================================

add_library(viscore STATIC
    gmod/GmodNodeResolution.cpp
    gmod/NodeCategory.cpp
)

set_target_properties(viscore
    PROPERTIES
        DEBUG_POSTFIX "${QVIS_TOOLS_DEBUG_POSTFIX}"
)

target_include_directories(viscore
    PUBLIC
        ${QVIS_TOOLS_SOURCE_DIR}
)

target_link_libraries(viscore
    PUBLIC
        dnv::vista::sdk
    PRIVATE
        qvistools::warnings
)
