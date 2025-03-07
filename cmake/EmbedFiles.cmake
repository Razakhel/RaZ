# Allows to create embeddable (#include-able) files directly in code

function (embed_files)
    set(options)
    set(oneValueArgs INPUT_PATTERN OUTPUT_FOLDER MAIN_TARGET EMBED_TARGET_SUFFIX)
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
            message(FATAL_ERROR "Embedding files requires a value for the '${REQUIRED_ARG}' argument")
        endif ()
    endforeach ()

    set(EMBED_TARGET_NAME "${arg_MAIN_TARGET}_Embed${arg_EMBED_TARGET_SUFFIX}")
    set(EMBED_SCRIPT "${CMAKE_BINARY_DIR}/${EMBED_TARGET_NAME}.cmake")

    # Emptying the embedding script
    file(WRITE "${EMBED_SCRIPT}" "")

    file(GLOB EMBEDDABLE_FILES "${arg_INPUT_PATTERN}")

    # Creating a copy of each file to enclose its content with a raw string literal R"(...)"
    foreach (FILE_PATH ${EMBEDDABLE_FILES})
        get_filename_component(FILE_NAME "${FILE_PATH}" NAME)
        set(EMBED_FILE_PATH "${arg_OUTPUT_FOLDER}/${FILE_NAME}.embed")

        # CMake cannot execute a script from a string and requires an actual file to be executed
        # The embeddable file will be written only if the original has been modified
        file(
            APPEND
            "${EMBED_SCRIPT}"

            "
                if (NOT EXISTS \"${EMBED_FILE_PATH}\" OR \"${FILE_PATH}\" IS_NEWER_THAN \"${EMBED_FILE_PATH}\")
                    file(READ \"${FILE_PATH}\" FILE_CONTENT)
                    file(WRITE \"${EMBED_FILE_PATH}\" \"R\\\"(\${FILE_CONTENT})\\\"\")
                endif ()
            "
        )
    endforeach ()

    add_custom_target(
        ${EMBED_TARGET_NAME}
        COMMAND ${CMAKE_COMMAND} -P "${EMBED_SCRIPT}" # Executing the previously created script
        VERBATIM
    )

    add_dependencies(${arg_MAIN_TARGET} ${EMBED_TARGET_NAME})

    # Adding the embedded files directory to the include dirs, so that we can include them directly in code
    target_include_directories(${arg_MAIN_TARGET} PUBLIC "${arg_OUTPUT_FOLDER}")
endfunction ()
