#==============================================================================
# QVisTools - Tests
#==============================================================================

find_package(doctest 2.5.3)

if(doctest_FOUND)
    message(STATUS "Found doctest: ${doctest_DIR}")
else()
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

    if(NOT TARGET doctest::doctest)
        message(FATAL_ERROR "doctest was fetched but target 'doctest::doctest' was not defined")
    endif()

    list(APPEND CMAKE_MODULE_PATH "${doctest_SOURCE_DIR}/scripts/cmake")
endif()

include(doctest)

add_subdirectory(viscore)
