# Copyright (c) 2026 AIperture-Labs <xavier.beheydt@gmail.com>
# Compile Commands Symlink Configuration
# Docs: 
#   - https://cmake.org/cmake/help/v4.1/index.html
#   - https://cliutils.gitlab.io/modern-cmake/README.html

#[=[
setup_compile_commands_symlink()

Create a symlink or copy of compile_commands.json from build to source directory.

This function ensures that the ``compile_commands.json`` file (generated during
configuration) is accessible in the source directory root. This enables IDE
integration and code analysis tools like ``clangd``, ``clang-tidy``, and ``VSCode``
to work correctly without additional configuration.

Behavior:
  1. Attempts to create a symlink from build directory to source directory
  2. On Windows with insufficient privileges, falls back to copying the file
  3. Subsequent builds update the copy via ``copy_if_different``

Prerequisites:
  - ``CMAKE_BINARY_DIR``  : Build directory (where compile_commands.json is generated)
  - ``CMAKE_SOURCE_DIR``  : Source directory (where symlink/copy will be created)

Usage:
  setup_compile_commands_symlink()  # Call in Config.cmake or main CMakeLists.txt

Output:
  - Prints status messages (symlink creation or fallback to copy)
  - Creates ``compile_commands.json`` in source directory

Notes:
  - This function is typically called from ``Config.cmake`` with conditional logic
  - The file should be added to ``.gitignore`` to avoid committing build artifacts
  - Compatible tools: ``clangd``, ``clang-tidy``, ``ccls``, ``rtags``
]=]
function(setup_compile_commands_symlink)
    set(CCDB_SRC "${CMAKE_BINARY_DIR}/compile_commands.json")
    set(CCDB_DST "${CMAKE_SOURCE_DIR}/compile_commands.json")

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CCDB_SRC}"
        "${CCDB_DST}"
        RESULT_VARIABLE SYMLINK_RESULT
        ERROR_QUIET
    )

    if(NOT SYMLINK_RESULT EQUAL 0)
        message(STATUS
            "Symlink for compile_commands.json failed (Windows privileges). "
            "Falling back to copy."
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CCDB_SRC}"
            "${CCDB_DST}"
        )
    else()
        message(STATUS
            "[setup_compile_commands_symlink] Created symlink for compile_commands.json: ${CCDB_DST} -> ${CCDB_SRC}"
        )
    endif()
endfunction()
