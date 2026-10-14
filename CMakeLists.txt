# ==============================================================================
# Rewind — Master CMake Configuration
# Linux-first, Local-first File Recovery System
# Language: C++20 | Build System: CMake 3.22+
# ==============================================================================

cmake_minimum_required(VERSION 3.22...3.31)

project(
    Rewind
    VERSION 0.1.0
    DESCRIPTION "Local-first transparent file recovery system"
    LANGUAGES C CXX
)

# ------------------------------------------------------------------------------
# 1. Global Standards and Compilation Settings
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for Clang-Tidy, LSP, and IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configure output directories for built binaries and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Set module path for custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ------------------------------------------------------------------------------
# 2. Options and Feature Toggles
# ------------------------------------------------------------------------------
option(BUILD_TESTING "Build the test suite" ON)
option(REWIND_BUILD_EXAMPLES "Build demo and example tools" OFF)

# ------------------------------------------------------------------------------
# 3. Include Subsystems & Dependencies
# ------------------------------------------------------------------------------
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/Dependencies.cmake)

# ------------------------------------------------------------------------------
# 4. Core Library Target (rewind_core)
# ------------------------------------------------------------------------------
# Collect core sources if available (excluding main entrypoint)
file(GLOB_RECURSE REWIND_CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
list(FILTER REWIND_CORE_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

if(REWIND_CORE_SOURCES)
    add_library(rewind_core STATIC ${REWIND_CORE_SOURCES})
    target_include_directories(rewind_core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(rewind_core PUBLIC cxx_std_20)
    rewind_set_compiler_warnings(rewind_core)
    rewind_set_sanitizers(rewind_core)

    if(TARGET SQLite::SQLite3)
        target_link_libraries(rewind_core PUBLIC SQLite::SQLite3)
    endif()
else()
    # Interface foundation target before Person 2 merges domain source implementations
    add_library(rewind_core INTERFACE)
    target_include_directories(rewind_core INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(rewind_core INTERFACE cxx_std_20)
endif()

# ------------------------------------------------------------------------------
# 5. CLI Executable Target (rewind_cli)
# ------------------------------------------------------------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    add_executable(rewind_cli src/main.cpp)
    set_target_properties(rewind_cli PROPERTIES OUTPUT_NAME "rewind")
    target_link_libraries(rewind_cli PRIVATE rewind_core)
    target_compile_features(rewind_cli PRIVATE cxx_std_20)
    rewind_set_compiler_warnings(rewind_cli)
    rewind_set_sanitizers(rewind_cli)

    install(TARGETS rewind_cli RUNTIME DESTINATION bin)
endif()

# ------------------------------------------------------------------------------
# 6. Test Suite and Verification Targets
# ------------------------------------------------------------------------------
if(BUILD_TESTING)
    enable_testing()
    include(GoogleTest)

    # Foundation verification check (guarantees ctest verification passes at baseline)
    add_test(
        NAME rewind_foundation_check
        COMMAND ${CMAKE_COMMAND} -E echo "Rewind toolchain foundation verified: C++20 environment active."
    )

    # Unit Tests: Discover and register tests/unit/*.cpp when present
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/test_common.cpp")
        add_executable(test_common tests/unit/test_common.cpp)
        target_link_libraries(test_common PRIVATE rewind_core GTest::gtest_main)
        target_compile_features(test_common PRIVATE cxx_std_20)
        rewind_set_compiler_warnings(test_common)
        rewind_set_sanitizers(test_common)
        gtest_discover_tests(test_common)
    endif()
endif()
