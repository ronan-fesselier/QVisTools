#==============================================================================
# viscore - Tests
#==============================================================================

file(GLOB_RECURSE VIS_CORE_TESTS_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(viscore-tests ${VIS_CORE_TESTS_SOURCES})

set_target_properties(viscore-tests
    PROPERTIES
        CXX_STANDARD          20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS        OFF
        DEBUG_POSTFIX         "${QVIS_TOOLS_DEBUG_POSTFIX}"
)

target_compile_definitions(viscore-tests
    PRIVATE
        DOCTEST_CONFIG_USE_STD_HEADERS
)

if(NOT TARGET doctest::doctest_with_main)
    set(VIS_CORE_DOCTEST_MAIN_CPP "${CMAKE_CURRENT_BINARY_DIR}/doctest_main.cpp")
    file(WRITE "${VIS_CORE_DOCTEST_MAIN_CPP}"
        "#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN\n#include <doctest/doctest.h>\n")
    target_sources(viscore-tests PRIVATE "${VIS_CORE_DOCTEST_MAIN_CPP}")
endif()

target_link_libraries(viscore-tests
    PRIVATE
        $<IF:$<TARGET_EXISTS:doctest::doctest_with_main>,doctest::doctest_with_main,doctest::doctest>
        viscore
        qvistools::warnings
)

doctest_discover_tests(viscore-tests)
