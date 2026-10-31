#==============================================================================
# QVisTools - Tests
#==============================================================================

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET                OFF)

include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.5.3
    GIT_SHALLOW    TRUE
)

block(SCOPE_FOR VARIABLES)
    set(BUILD_SHARED_LIBS OFF)
    FetchContent_MakeAvailable(doctest)
endblock()

list(APPEND CMAKE_MODULE_PATH "${doctest_SOURCE_DIR}/scripts/cmake")

include(doctest)

add_subdirectory(core)
