# ==============================================================================
# Rewind — Third-Party Dependencies (GTest, SQLite3)
# ==============================================================================

include(FetchContent)

# ------------------------------------------------------------------------------
# 1. GoogleTest (Testing Framework)
# ------------------------------------------------------------------------------
if(BUILD_TESTING)
    find_package(GTest QUIET)
    if(TARGET GTest::gtest AND TARGET GTest::gtest_main)
        message(STATUS "Found installed GoogleTest: ${GTest_VERSION}")
    else()
        message(STATUS "GoogleTest not found locally. Fetching via FetchContent (v1.14.0)...")
        if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24")
            FetchContent_Declare(
                googletest
                URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
                DOWNLOAD_EXTRACT_TIMESTAMP TRUE
            )
        else()
            FetchContent_Declare(
                googletest
                URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
            )
        endif()
        # Prevent GTest from overriding compiler/linker flags or installing
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

        FetchContent_MakeAvailable(googletest)

        # Mark GoogleTest include directories as SYSTEM to suppress external warnings
        if(TARGET gtest)
            set_target_properties(gtest PROPERTIES SYSTEM TRUE)
        endif()
        if(TARGET gtest_main)
            set_target_properties(gtest_main PROPERTIES SYSTEM TRUE)
        endif()
    endif()
endif()

# ------------------------------------------------------------------------------
# 2. SQLite3 (Transactional Metadata Engine)
# ------------------------------------------------------------------------------
find_package(SQLite3 QUIET)
if(TARGET SQLite::SQLite3)
    message(STATUS "Found system SQLite3: ${SQLite3_VERSION}")
else()
    message(STATUS "System SQLite3 not found; SQLite3 target will be resolved via package manager or fallback when required.")
endif()
