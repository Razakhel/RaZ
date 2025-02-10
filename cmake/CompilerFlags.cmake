# Allows to define useful compiler flags (warnings, needed definitions, ...)

function(add_compiler_flags)
    set(options)
    set(oneValueArgs TARGET SCOPE)
    set(multiValueArgs)
    cmake_parse_arguments(
        PARSE_ARGV 0
        arg
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
    )
    foreach (REQUIRED_ARG IN LISTS oneValueArgs)
        if (NOT arg_${REQUIRED_ARG})
            message(FATAL_ERROR "Adding compiler flags requires a value for the '${REQUIRED_ARG}' argument")
        endif ()
    endforeach ()

    # The definitions MUST be propagated to avoid warnings and/or errors in headers
    set(DEFINITIONS_SCOPE ${arg_SCOPE})
    if (DEFINITIONS_SCOPE STREQUAL "PRIVATE")
        set(DEFINITIONS_SCOPE "PUBLIC")
    endif ()

    # Determining the compiler used

    if (CMAKE_CXX_COMPILER MATCHES "/em\\+\\+.*$") # Emscripten
        set(COMPILER_EMSCRIPTEN ON)
    elseif (MINGW)
        set(COMPILER_MINGW ON)
    elseif (MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang") # MSVC
        set(COMPILER_MSVC ON)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang") # Clang
        set(COMPILER_CLANG ON)
        if (MSVC) # Clang-cl
            set(COMPILER_CLANG_CL ON)
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU") # GCC
        set(COMPILER_GCC ON)
    endif ()

    if (COMPILER_GCC)
        list(
            APPEND
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
            -Wno-missing-field-initializers
        )

        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
            list(
                APPEND
                COMPILER_FLAGS

                -fsized-deallocation
                -Warray-bounds=2
                -Wformat-signedness
                -Wsized-deallocation
            )
        endif ()

        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6)
            list(
                APPEND
                COMPILER_FLAGS

                -Wduplicated-cond
                #-Wnull-dereference # Lua bindings (Sol) generate a lot of these
            )
        endif ()

        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7)
            list(
                APPEND
                COMPILER_FLAGS

                -Waligned-new
                -Walloca
                -Walloc-zero
                -Wformat-overflow
                -Wshadow
            )
        endif ()
    elseif (COMPILER_CLANG)
        list(
            APPEND
            COMPILER_FLAGS

            -Weverything

            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-covered-switch-default
            -Wno-ctad-maybe-unsupported
            -Wno-documentation
            -Wno-documentation-unknown-command
            -Wno-exit-time-destructors
            -Wno-float-equal
            -Wno-format-nonliteral
            -Wno-global-constructors
            -Wno-mismatched-tags
            -Wno-missing-braces
            -Wno-missing-field-initializers
            -Wno-newline-eof
            -Wno-padded
            -Wno-reserved-id-macro
            -Wno-sign-conversion
            -Wno-switch-enum
            -Wno-weak-vtables
        )

        if (COMPILER_CLANG_CL)
            list(
                APPEND
                COMPILER_FLAGS

                # Disabling warnings triggered in externals
                -Wno-language-extension-token
                -Wno-nonportable-system-include-path
                -Wno-zero-as-null-pointer-constant
            )
        else ()
            list(
                APPEND
                COMPILER_FLAGS

                # Other flags not recognized by clang-cl
                -pedantic
                -pedantic-errors
            )
        endif ()

        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
            list(APPEND COMPILER_FLAGS -Wno-unused-template)
        endif ()

        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16)
            list(APPEND COMPILER_FLAGS -Wno-unsafe-buffer-usage)
        endif ()
    elseif (COMPILER_MSVC)
        list(
            APPEND
            COMPILER_FLAGS

            /Wall
            /MP # Enabling multi-processes compilation

            /wd4061 # Enum value in a switch not explicitly handled by a case label
            /wd4242 # Conversion from T1 to T2, possible loss of data (any type)
            /wd4244 # Conversion from T1 to T2, possible loss of data (specific types)
            /wd4312 # Conversion from T1 to T2 of greater size
            /wd4571 # SEH exceptions aren't caught since Visual C++ 7.1
            /wd5045 # Spectre mitigation
            /wd5246 # Initialization of subobject should be wrapped in braces
            /wd5264 # const(expr) variable not used

            # Warnings triggered by Sol
            /wd4371 # Class layout may have changed from a previous compiler version due to better packing
            /wd4582 # Constructor not implicitly called
            /wd4583 # Destructor not implicitly called
            /wd5243 # Using incomplete class can cause ODR violation

            # Warnings triggered by the FBX SDK
            /wd4266 # No override available (function is hidden)
            /wd4365 # Signed/unsigned mismatch (implicit conversion)
            /wd4619 # Unknown warning number
            /wd4625 # Copy constructor implicitly deleted
            /wd4626 # Copy assignment operator implicitly deleted

            # Warnings triggered by MSVC's standard library
            /wd4355 # 'this' used in base member initializing list
            /wd4514 # Unreferenced inline function has been removed
            /wd4548 # Expression before comma has no effect
            /wd4623 # Default constructor was implicitly defined as deleted
            /wd4668 # Preprocessor macro not defined
            /wd4710 # Function not inlined
            /wd4711 # Function inlined
            /wd4774 # Format string is not a string literal
            /wd4820 # Added padding to members
            /wd5026 # Move constructor implicitly deleted
            /wd5027 # Move assignment operator implicitly deleted
            /wd5039 # Pointer/ref to a potentially throwing function passed to an 'extern "C"' function (with -EHc)
            /wd5105 # Undefined macro expansion
            /wd5204 # Class with virtual functions but no virtual destructor
            /wd5220 # Non-static volatile member doesn't imply non-trivial move/copy ctor/operator=
            /wd5262 # Implicit fallthrough
        )

        # Automatically export all classes & functions
        set_property(TARGET ${arg_TARGET} PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif ()

    if (COMPILER_MINGW)
        list(
            APPEND
            COMPILER_FLAGS

            -Wa,-mbig-obj # Allowing big object files
            -fuse-ld=lld # Using LLVM LLD as the linker
        )
    endif ()

    if (COMPILER_MSVC OR COMPILER_CLANG_CL)
        list(
            APPEND
            COMPILER_FLAGS

            /permissive- # Improving standard compliance
            /bigobj # Allowing object files to be bigger
            /EHsc # Enabling exceptions
            /utf-8 # Forcing MSVC to actually handle files as UTF-8

            # Forcing inline functions to have their definition in each translation units they're called from (required from C++11 onward)
            # See: https://learn.microsoft.com/en-us/cpp/build/reference/zc-inline-remove-unreferenced-comdat
            /Zc:inline

            # Forcing the compiler to consider the operator new always potentially throwing, allowing several optimizations
            # See: https://learn.microsoft.com/en-us/cpp/build/reference/zc-throwingnew-assume-operator-new-throws
            /Zc:throwingNew

            # Adds carets to the compiler's error output, which can make its messages more readable
            # See: https://learn.microsoft.com/en-us/cpp/build/reference/diagnostics-compiler-diagnostic-options
            /diagnostics:caret
        )

        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.13)
            list(
                APPEND
                COMPILER_FLAGS

                # Allowing external linkage for 'extern constexpr' variables
                # See: https://learn.microsoft.com/en-us/cpp/build/reference/zc-externconstexpr
                /Zc:externConstexpr
            )
        endif ()

        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
            list(
                APPEND
                COMPILER_FLAGS

                # Forcing the '__cplusplus' definition to be of the proper value
                # See: https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus
                /Zc:__cplusplus
            )
        endif ()

        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.25)
            list(
                APPEND
                COMPILER_FLAGS

                # Forcing the preprocessor to be compliant with C++11 and above
                # See: https://learn.microsoft.com/en-us/cpp/build/reference/zc-preprocessor
                /Zc:preprocessor
            )
        endif ()

        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.28)
            list(
                APPEND
                COMPILER_FLAGS

                # Forcing lambdas' parsing to be standard compliant
                # To be removed in C++20 (implied by /std:c++20)
                # See: https://learn.microsoft.com/en-us/cpp/build/reference/zc-lambda
                /Zc:lambda
            )
        endif ()

        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.34)
            list(
                APPEND
                COMPILER_FLAGS

                # Forcing standard enumeration type deduction
                # See: https://learn.microsoft.com/en-us/cpp/build/reference/zc-enumtypes
                /Zc:enumTypes
            )
        endif ()

        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.35)
            list(
                APPEND
                COMPILER_FLAGS

                # Forcing template parameters' names to not be reused (shadowed)
                # See: https://learn.microsoft.com/en-us/cpp/build/reference/zc-templatescope
                /Zc:templateScope
            )
        endif ()
    endif ()

    if (NOT COMPILER_MSVC)
        # Defining the compiler flags only for C++; this doesn't work with MSVC
        set(COMPILER_FLAGS $<$<COMPILE_LANGUAGE:CXX>:${COMPILER_FLAGS}>)
    endif ()

    if (WIN32 OR CYGWIN)
        target_compile_definitions(
            ${arg_TARGET}

            ${DEFINITIONS_SCOPE}

            NOMINMAX # Preventing definitions of min & max macros
            NOGDI # Preventing definition of the 'ERROR' macro
            _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING # Ignoring std::codecvt deprecation warnings
        )
    endif ()

    if (COMPILER_EMSCRIPTEN)
        # List of all compiler & linker options: https://emscripten.org/docs/tools_reference/emcc.html
        # List of all -s options: https://github.com/emscripten-core/emscripten/blob/main/src/settings.js

        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            option(RAZ_EMSCRIPTEN_DEBUG_FAST_LINK "Allows to drastically speed up linking time in Debug; do not use this for an actual debug build" OFF)

            if (RAZ_EMSCRIPTEN_DEBUG_FAST_LINK)
                # See https://emscripten.org/docs/optimizing/Optimizing-Code.html#link-times for details on improving link times

                target_link_options(
                    ${arg_TARGET}

                    ${arg_SCOPE}

                    "SHELL:-s ERROR_ON_WASM_CHANGES_AFTER_LINK" # Produces a linking error if the linker requires modifying the generated WASM
                    "SHELL:-s WASM_BIGINT" # Enabling BigInt support
                )
            else ()
                list(
                    APPEND
                    COMPILER_FLAGS

                    -g
                    #-fsanitize=address,undefined # Enable sanitizers
                )

                target_link_options(
                    ${arg_TARGET}

                    ${arg_SCOPE}

                    #-fsanitize=address,undefined # Enable sanitizers
                    "SHELL:-s ASSERTIONS=1" # Enable assertions
                    "SHELL:-s GL_ASSERTIONS=1" # Enable OpenGL error checks
                    #"SHELL:-s GL_TESTING=1" # Keep the drawing buffer alive to allow testing
                    #"SHELL:-s INITIAL_MEMORY=300MB" # Expanding the initial memory pool; required for asan
                    "SHELL:-s SAFE_HEAP=1" # Enable heap checks
                    "SHELL:-s STACK_OVERFLOW_CHECK=2" # Enhance call stack precision
                )
            endif ()

            list(APPEND COMPILER_FLAGS -O0)

            target_link_options(
                ${arg_TARGET}

                ${arg_SCOPE}

                --emrun # Forward stdout & stderr to the launching console
                --cpuprofiler # Display a CPU profiler on the page
                #--memoryprofiler # Display a memory profiler on the page (SAFE_HEAP needs to be disabled: "maximum call stack size exceeded")
                #-gsource-map # Generate source map
            )
        else ()
            target_compile_definitions(${arg_TARGET} ${DEFINITIONS_SCOPE} NDEBUG)
            list(APPEND COMPILER_FLAGS -O3)
        endif ()

        # Threading is available on Emscripten's side (see arguments below), but may not be on the browser's. This will need to be checked again in the future
        # See: https://emscripten.org/docs/porting/pthreads.html

        list(
            APPEND
            COMPILER_FLAGS

            -c # Emit object files (may be unrequired)
            #-pthread # Enabling pthread
            "SHELL:-s DISABLE_EXCEPTION_CATCHING=0" # Force catching exceptions
        )

        target_link_options(
            ${arg_TARGET}

            ${arg_SCOPE}

            #-pthread # Enabling pthread
            "SHELL:-s ALLOW_MEMORY_GROWTH=1" # Automatically reallocate memory if needed
            "SHELL:-s DISABLE_EXCEPTION_CATCHING=0" # Force catching exceptions
            "SHELL:-s OFFSCREEN_FRAMEBUFFER=1" # Enable rendering to offscreen targets
            "SHELL:-s OFFSCREENCANVAS_SUPPORT=1" # Allow creating multiple GL contexts on separate threads & swapping between them
            #"SHELL:-s PROXY_TO_PTHREAD" # Runs the whole program into a separate thread
            #"SHELL:-s PTHREAD_POOL_SIZE=navigator.hardwareConcurrency" # Setting the initial thread pool size to the system's thread count
            "SHELL:-s USE_WEBGL2=1" # Force the use of WebGL2
        )
    endif ()

    target_compile_options(${arg_TARGET} ${arg_SCOPE} ${COMPILER_FLAGS})

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Enabling assertions in the standard library in Debug mode
        # The files in which the symbols are checked are those where the symbols are directly defined. They do exist at the time of writing, but that may
        #  not always be so. Should that not be the case, the symbols will be considered nonexistent and the behavior will remain the default
        include(CheckCXXSymbolExists)
        check_cxx_symbol_exists("__GLIBCXX__" "bits/c++config.h" USING_LIBSTDCPP)
        check_cxx_symbol_exists("_LIBCPP_VERSION" "__config" USING_LIBCPP)

        if (USING_LIBSTDCPP)
            # See https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_macros.html
            target_compile_definitions(${arg_TARGET} ${DEFINITIONS_SCOPE} _GLIBCXX_ASSERTIONS)

            # libstdc++ also supports a "debug mode", with optional backtrace reporting. This changes the size and behavior of types and so isn't enabled
            #  by default here, but can be useful in certain cases. This implies _GLIBCXX_ASSERTIONS
            # See https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug_mode_using.html#debug_mode.using.mode
            #target_compile_definitions(${arg_TARGET} ${DEFINITIONS_SCOPE} _GLIBCXX_DEBUG _GLIBCXX_DEBUG_BACKTRACE)
            #target_link_libraries(${arg_TARGET} ${DEFINITIONS_SCOPE} stdc++exp) # Required for the backtrace support
        elseif (USING_LIBCPP)
            # Enabling hardening mode; see https://libcxx.llvm.org/Hardening.html
            # The extensive mode's checks may be enough, but only the debug one seems to output error messages on assertions
            target_compile_definitions(${arg_TARGET} ${DEFINITIONS_SCOPE} _LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG)
        endif ()
    endif ()
endfunction()
