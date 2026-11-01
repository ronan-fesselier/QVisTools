#==============================================================================
# QVisTools - core
#==============================================================================

add_library(qvistools-core STATIC
    vis/localid/LocalId.cpp

    vis/gmod/AncestorPath.cpp
    vis/gmod/GmodNodeChildren.cpp
    vis/gmod/NodeCategory.cpp
    vis/gmod/SearchIndex.cpp
)
add_library(qvistools::core ALIAS qvistools-core)

set_target_properties(qvistools-core
    PROPERTIES
        DEBUG_POSTFIX "${QVIS_TOOLS_DEBUG_POSTFIX}"
)

target_include_directories(qvistools-core
    PUBLIC
        ${QVIS_TOOLS_SOURCE_DIR}
)

target_link_libraries(qvistools-core
    PUBLIC
        dnv::vista::sdk
    PRIVATE
        qvistools::warnings
)
