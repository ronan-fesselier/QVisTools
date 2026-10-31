#==============================================================================
# QVisTools - CMake targets
#==============================================================================

#----------------------------------------------
# Warnings
#----------------------------------------------

if(QVIS_TOOLS_WARNINGS_AS_ERRORS AND NOT QVIS_TOOLS_WARNINGS)
    set(QVIS_TOOLS_WARNINGS ON CACHE BOOL "Enable strict compiler warnings" FORCE)
endif()

add_library(QVisTools-warnings INTERFACE)
add_library(QVisTools::warnings ALIAS QVisTools-warnings)

target_compile_options(QVisTools-warnings
    INTERFACE
        $<$<AND:$<BOOL:${QVIS_TOOLS_WARNINGS}>,$<CXX_COMPILER_ID:GNU,Clang>>:-Wall -Wextra -Wpedantic>
        $<$<AND:$<BOOL:${QVIS_TOOLS_WARNINGS}>,$<CXX_COMPILER_ID:MSVC>>:/W4>
        $<$<AND:$<BOOL:${QVIS_TOOLS_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:GNU,Clang>>:-Werror>
        $<$<AND:$<BOOL:${QVIS_TOOLS_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:MSVC>>:/WX>
        $<$<CXX_COMPILER_ID:MSVC>:/permissive- /utf-8>
)

#----------------------------------------------
# Sources
#----------------------------------------------

set(APP_SOURCES
    src/main.cpp
)

#----------------------------------------------
# Executable
#----------------------------------------------

add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        QVisTools::warnings
)
