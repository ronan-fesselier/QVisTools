#==============================================================================
# qvistools::core - Tests
#==============================================================================

file(GLOB_RECURSE QVIS_TOOLS_CORE_TESTS_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(qvistools-core-tests ${QVIS_TOOLS_CORE_TESTS_SOURCES})

set_target_properties(qvistools-core-tests
    PROPERTIES
        CXX_STANDARD          20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS        OFF
        DEBUG_POSTFIX         "${QVIS_TOOLS_DEBUG_POSTFIX}"
)

target_compile_definitions(qvistools-core-tests
    PRIVATE
        DOCTEST_CONFIG_USE_STD_HEADERS
)

target_link_libraries(qvistools-core-tests
    PRIVATE
        doctest::doctest_with_main
        qvistools::core
        qvistools::warnings
)

doctest_discover_tests(qvistools-core-tests)
