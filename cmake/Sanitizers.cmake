# ==============================================================================
# Rewind — Runtime Sanitizers Configuration (ASan, UBSan, TSan)
# ==============================================================================

option(REWIND_ENABLE_ASAN "Enable AddressSanitizer and LeakSanitizer" OFF)
option(REWIND_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(REWIND_ENABLE_TSAN "Enable ThreadSanitizer" OFF)

function(rewind_set_sanitizers TARGET_NAME)
    if(NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR "Target '${TARGET_NAME}' does not exist for sanitizer configuration.")
    endif()

    if(REWIND_ENABLE_ASAN AND REWIND_ENABLE_TSAN)
        message(FATAL_ERROR "AddressSanitizer (ASan) and ThreadSanitizer (TSan) cannot be enabled simultaneously.")
    endif()

    set(SANITIZER_COMPILE_FLAGS "")
    set(SANITIZER_LINK_FLAGS "")

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(REWIND_ENABLE_ASAN)
            list(APPEND SANITIZER_COMPILE_FLAGS -fsanitize=address)
            list(APPEND SANITIZER_LINK_FLAGS -fsanitize=address)
            # LeakSanitizer is supported on Linux with GCC/Clang
            if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
                list(APPEND SANITIZER_COMPILE_FLAGS -fsanitize=leak)
                list(APPEND SANITIZER_LINK_FLAGS -fsanitize=leak)
            endif()
        endif()

        if(REWIND_ENABLE_UBSAN)
            list(APPEND SANITIZER_COMPILE_FLAGS -fsanitize=undefined)
            list(APPEND SANITIZER_LINK_FLAGS -fsanitize=undefined)
        endif()

        if(REWIND_ENABLE_TSAN)
            list(APPEND SANITIZER_COMPILE_FLAGS -fsanitize=thread)
            list(APPEND SANITIZER_LINK_FLAGS -fsanitize=thread)
        endif()

        if(SANITIZER_COMPILE_FLAGS)
            list(APPEND SANITIZER_COMPILE_FLAGS -fno-omit-frame-pointer -g)
            target_compile_options(${TARGET_NAME} PRIVATE ${SANITIZER_COMPILE_FLAGS})
            target_link_options(${TARGET_NAME} PRIVATE ${SANITIZER_LINK_FLAGS})
        endif()
    elseif(MSVC)
        if(REWIND_ENABLE_ASAN)
            target_compile_options(${TARGET_NAME} PRIVATE /fsanitize=address)
            target_compile_options(${TARGET_NAME} PRIVATE /Zi)
            target_link_options(${TARGET_NAME} PRIVATE /DEBUG)
        endif()
        if(REWIND_ENABLE_UBSAN OR REWIND_ENABLE_TSAN)
            message(WARNING "UBSan and TSan are not supported under MSVC.")
        endif()
    endif()
endfunction()
