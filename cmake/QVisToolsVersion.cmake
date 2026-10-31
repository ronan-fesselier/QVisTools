#==============================================================================
# QVisTools -  Version configuration
#==============================================================================

set(QVIS_TOOLS_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE INTERNAL "QVisTools major version")
set(QVIS_TOOLS_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE INTERNAL "QVisTools minor version")
set(QVIS_TOOLS_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE INTERNAL "QVisTools patch version")
set(QVIS_TOOLS_VERSION       ${PROJECT_VERSION}       CACHE INTERNAL "QVisTools version")

set(QVIS_TOOLS_GENERATED_INCLUDE_DIR "${PROJECT_BINARY_DIR}/include" CACHE INTERNAL
    "QVisTools generated headers include directory")

configure_file(
    "${PROJECT_SOURCE_DIR}/src/Version.h.in"
    "${QVIS_TOOLS_GENERATED_INCLUDE_DIR}/qvistools/Version.h"
    @ONLY
)
