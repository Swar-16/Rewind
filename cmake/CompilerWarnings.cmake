# ==============================================================================
# Rewind — Strict Compiler Warnings Configuration
# Modern C++20 Standards Enforcer
# ==============================================================================

option(REWIND_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

function(rewind_set_compiler_warnings TARGET_NAME)
    if(NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR "Target '${TARGET_NAME}' does not exist for compiler warnings configuration.")
    endif()

    set(CLANG_WARNINGS
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
        -Wconversion
        -Wsign-conversion
    )

    set(GCC_WARNINGS
        ${CLANG_WARNINGS}
        -Wlogical-op
        -Wduplicated-cond
        -Wduplicated-branches
        -Wuseless-cast
    )

    set(MSVC_WARNINGS
        /W4
        /permissive-
        /w14242 # 'identfier': conversion from 'type1' to 'type1', possible loss of data
        /w14254 # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits'
        /w14263 # 'function': member function does not override any base class virtual member function
        /w14265 # 'class': class has virtual functions, but destructor is not virtual
        /w14287 # 'operator': unsigned/negative constant mismatch
        /w14296 # 'operator': expression is always 'boolean_value'
        /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
        /w14545 # expression before comma evaluates to a function which is missing an argument list
        /w14546 # function call before comma missing argument list
        /w14547 # 'operator': operator before comma has no effect; expected operator with side-effect
        /w14549 # 'operator': operator before comma has no effect; did you intend 'operator'?
        /w14555 # expression has no effect; expected expression with side-effect
        /w14619 # pragma warning: there is no warning number 'number'
        /w14640 # Enable warning on thread un-safe static member initialization
        /w14826 # Conversion from 'type1' to 'type_2' is sign-extended
        /w14905 # Wide string literal cast to 'LPSTR'
        /w14906 # String literal cast to 'LPWSTR'
        /w14928 # Illegal copy-initialization; more than one user-defined conversion has been implicitly applied
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${TARGET_NAME} PRIVATE ${CLANG_WARNINGS})
        if(REWIND_WARNINGS_AS_ERRORS)
            target_compile_options(${TARGET_NAME} PRIVATE -Werror)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${TARGET_NAME} PRIVATE ${GCC_WARNINGS})
        if(REWIND_WARNINGS_AS_ERRORS)
            target_compile_options(${TARGET_NAME} PRIVATE -Werror)
        endif()
    elseif(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE ${MSVC_WARNINGS})
        if(REWIND_WARNINGS_AS_ERRORS)
            target_compile_options(${TARGET_NAME} PRIVATE /WX)
        endif()
    else()
        message(AUTHOR_WARNING "No compiler warning flags configured for '${CMAKE_CXX_COMPILER_ID}'.")
    endif()
endfunction()
