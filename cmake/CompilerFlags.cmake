# Allows to define useful compiler flags (warnings, needed definitions, ...)

function(add_compiler_flags TARGET_NAME SCOPE)
    # The definitions MUST be propagated to avoid warnings and/or errors in headers
    set(DEFINITIONS_SCOPE ${SCOPE})
    if (DEFINITIONS_SCOPE STREQUAL "PRIVATE")
        set(DEFINITIONS_SCOPE "PUBLIC")
    endif ()

    if (RAZ_COMPILER_GCC)
        set(
            COMPILER_FLAGS

            -pedantic
            -pedantic-errors
            -Wall
            -Wextra

            -Warray-bounds
            -Wcast-align
            -Wcast-qual
            -Wconditionally-supported
            -Wconversion
            -Wdisabled-optimization
            -Wdouble-promotion
            -Wfloat-conversion
            -Wformat=2
            -Wformat-security
            -Wlogical-op
            -Wmissing-declarations
            -Wmissing-include-dirs
            -Wnoexcept
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wopenmp-simd
            -Woverloaded-virtual
            -Wpacked
            -Wredundant-decls
            -Wstrict-aliasing
            -Wstrict-null-sentinel
            #-Wsuggest-final-methods
            #-Wsuggest-final-types
            -Wtrampolines
            -Wundef
            -Wuninitialized
            -Wunused-macros
            -Wuseless-cast
            -Wvector-operation-performance
            -Wvla
            -Wzero-as-null-pointer-constant

            -Wno-comment
            -Wno-format-nonliteral
        )

        # Enabling some other warnings available since GCC 5
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
            set(
                COMPILER_FLAGS

                ${COMPILER_FLAGS}
                -fsized-deallocation
                -Warray-bounds=2
                -Wformat-signedness
                -Wsized-deallocation
            )
        endif ()

        # Enabling some other warnings available since GCC 6
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6)
            set(
                COMPILER_FLAGS

                ${COMPILER_FLAGS}
                -Wduplicated-cond
                -Wnull-dereference
            )
        endif ()

        # Enabling some other warnings available since GCC 7
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7)
            set(
                COMPILER_FLAGS

                ${COMPILER_FLAGS}
                -Waligned-new
                -Walloca
                -Walloc-zero
                -Wformat-overflow
                -Wshadow
            )
        endif ()
    elseif (RAZ_COMPILER_CLANG)
        set(
            COMPILER_FLAGS

            -Weverything

            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-covered-switch-default
            -Wno-documentation
            -Wno-documentation-unknown-command
            -Wno-exit-time-destructors
            -Wno-float-equal
            -Wno-format-nonliteral
            -Wno-global-constructors
            -Wno-mismatched-tags
            -Wno-missing-braces
            -Wno-padded
            -Wno-reserved-id-macro
            -Wno-sign-conversion
            -Wno-switch-enum
            -Wno-weak-vtables
        )

        if (RAZ_COMPILER_CLANG_CL)
            set(
                COMPILER_FLAGS

                ${COMPILER_FLAGS}
                # Disabling warnings triggered in externals
                -Wno-language-extension-token
                -Wno-nonportable-system-include-path
                -Wno-zero-as-null-pointer-constant
            )
        else ()
            set(
                COMPILER_FLAGS

                ${COMPILER_FLAGS}
                # Other warning flags not recognized by clang-cl
                -pedantic
                -pedantic-errors
            )
        endif ()

        # Disabling some warnings available since Clang 5
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
            set(
                COMPILER_FLAGS

                ${COMPILER_FLAGS}
                -Wno-unused-template
            )
        endif ()
    elseif (RAZ_COMPILER_MSVC)
        set(
            COMPILER_FLAGS

            /Wall
            /MP # Enabling multi-processes compilation

            /wd4061 # Enum value in a switch not explicitly handled by a case label
            /wd4571 # SEH exceptions aren't caught since Visual C++ 7.1
            /wd5045 # Spectre mitigation

            # Warnings triggered by the FBX SDK
            /wd4266 # No override available (function is hidden)
            /wd4619 # Unknown warning number
            /wd4625 # Copy constructor implicitly deleted
            /wd4626 # Copy assignment operator implicitly deleted

            # Warnings triggered by Catch
            /wd4365 # Signed/unsigned mismatch (implicit conversion)
            /wd4388 # Signed/unsigned mismatch (equality comparison)
            /wd4583 # Destructor not implicitly called
            /wd4623 # Default constructor implicitly deleted
            /wd4868 # Evaluation order not guaranteed in braced initializing list
            /wd5204 # Class with virtual functions but no virtual destructor

            # Warnings triggered by MSVC's standard library
            /wd4355 # 'this' used in base member initializing list
            /wd4514 # Unreferenced inline function has been removed
            /wd4548 # Expression before comma has no effect
            /wd4668 # Preprocessor macro not defined
            /wd4710 # Function not inlined
            /wd4711 # Function inlined
            /wd4774 # Format string is not a string literal
            /wd4820 # Added padding to members
            /wd5026 # Move constructor implicitly deleted
            /wd5027 # Move assignment operator implicitly deleted
            /wd5039 # Pointer/ref to a potentially throwing function passed to an 'extern "C"' function (with -EHc)
            /wd5220 # Non-static volatile member doesn't imply non-trivial move/copy ctor/operator=
        )

        # To automatically export all the classes & functions
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

        # CMake automatically appends /W3 to the standard flags, which produces a warning with MSVC when adding another level; this has to be removed
        # TODO: if possible, this should be done per target, not globally
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif ()

    if (RAZ_COMPILER_MSVC OR RAZ_COMPILER_CLANG_CL)
        set(
            COMPILER_FLAGS

            ${COMPILER_FLAGS}
            /permissive- # Improving standard compliance
            /EHsc # Enabling exceptions
            /utf-8 # Forcing MSVC to actually handle files as UTF-8
        )

        target_compile_definitions(
            ${TARGET_NAME}

            ${DEFINITIONS_SCOPE}

            NOMINMAX # Preventing definitions of min & max macros
            _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING # Ignoring std::codecvt deprecation warnings
        )
    endif ()

    if (NOT RAZ_COMPILER_MSVC)
        # Defining the compiler flags only for C++; this doesn't work with MSVC
        set(COMPILER_FLAGS $<$<COMPILE_LANGUAGE:CXX>:${COMPILER_FLAGS}>)
    endif ()

    if (RAZ_PLATFORM_WINDOWS)
        target_compile_definitions(
            ${TARGET_NAME}

            ${DEFINITIONS_SCOPE}

            NOGDI # Preventing definition of the 'ERROR' macro
        )
    endif ()

    target_compile_options(${TARGET_NAME} ${SCOPE} ${COMPILER_FLAGS})
endfunction()
