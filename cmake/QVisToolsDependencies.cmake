#==============================================================================
# QVisTools - Dependencies
#==============================================================================

#----------------------------------------------
# Qt6
#----------------------------------------------

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)

#----------------------------------------------
# dnv-vista-sdk
#----------------------------------------------

find_package(dnv-vista-sdk 0.3.3...<0.4)

if(dnv-vista-sdk_FOUND)
    message(STATUS "Found dnv-vista-sdk: ${dnv-vista-sdk_DIR}")
else()
    set(FETCHCONTENT_QUIET OFF)

    include(FetchContent)

    FetchContent_Declare(
        dnv-vista-sdk
        GIT_REPOSITORY https://github.com/ronan-fesselier/vista-sdk.git
        GIT_TAG        feature/cpp
        SOURCE_SUBDIR  cpp
    )

    block(SCOPE_FOR VARIABLES)
        set(BUILD_SHARED_LIBS ON)
        FetchContent_MakeAvailable(dnv-vista-sdk)
    endblock()

    if(NOT TARGET dnv::vista::sdk)
        message(FATAL_ERROR "dnv-vista-sdk was fetched but target 'dnv::vista::sdk' was not defined")
    endif()
endif()
