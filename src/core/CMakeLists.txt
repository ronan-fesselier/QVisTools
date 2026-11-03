#==============================================================================
# QVisTools - core
#==============================================================================

add_library(qvistools-core STATIC
    vis/codebooks/CodebookValues.cpp

    vis/localid/LocalId.cpp
    vis/localid/ParseId.cpp

    vis/gmod/AncestorPath.cpp
    vis/gmod/GmodNodeChildren.cpp
    vis/gmod/GmodPathLocation.cpp
    vis/gmod/NodeCategory.cpp
    vis/gmod/SearchIndex.cpp

    vis/universalid/UniversalId.cpp
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
