#==============================================================================
# QVisTools - Warnings
#==============================================================================

if(QVIS_TOOLS_WARNINGS_AS_ERRORS AND NOT QVIS_TOOLS_WARNINGS)
    set(QVIS_TOOLS_WARNINGS ON CACHE BOOL "Enable strict compiler warnings" FORCE)
endif()

add_library(qvistools-warnings INTERFACE)
add_library(qvistools::warnings ALIAS qvistools-warnings)

target_compile_options(qvistools-warnings
    INTERFACE
        $<$<AND:$<BOOL:${QVIS_TOOLS_WARNINGS}>,$<CXX_COMPILER_ID:GNU,Clang>>:-Wall -Wextra -Wpedantic>
        $<$<AND:$<BOOL:${QVIS_TOOLS_WARNINGS}>,$<CXX_COMPILER_ID:MSVC>>:/W4>
        $<$<AND:$<BOOL:${QVIS_TOOLS_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:GNU,Clang>>:-Werror>
        $<$<AND:$<BOOL:${QVIS_TOOLS_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:MSVC>>:/WX>
        $<$<CXX_COMPILER_ID:MSVC>:/permissive- /utf-8>
)
