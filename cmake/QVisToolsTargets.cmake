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
# Executable
#----------------------------------------------

add_executable(${PROJECT_NAME} WIN32
    ${QVIS_TOOLS_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${QVIS_TOOLS_SOURCE_DIR}
        ${QVIS_TOOLS_GENERATED_INCLUDE_DIR}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
            BUILD_RPATH   "$ORIGIN"
            INSTALL_RPATH "$ORIGIN"
    )
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Svg
        Qt6::Widgets
        dnv::vista::sdk
        QVisTools::warnings
)
