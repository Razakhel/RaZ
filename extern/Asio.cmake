########
# Asio #
########

project(Asio)

add_library(Asio INTERFACE)

file(GLOB_RECURSE ASIO_FILES asio/include/*.hpp)

target_sources(Asio INTERFACE ${ASIO_FILES})

target_include_directories(Asio SYSTEM INTERFACE asio/include)

target_compile_definitions(Asio INTERFACE ASIO_NO_DEPRECATED) # Remove deprecated features

if (RAZ_PLATFORM_WINDOWS)
    # Replicated the build script's value (Windows 8.1/WINBLUE): https://github.com/chriskohlhoff/asio/blob/231cb29bab30f82712fcd54faaea42424cc6e710/asio/src/Makefile.msc#L39
    # See https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt#remarks for possible values
    target_compile_definitions(Asio INTERFACE _WIN32_WINNT=0x0603)
endif ()
