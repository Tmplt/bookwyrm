find_package(CURL REQUIRED)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/components/command_line.cpp
    ${PROJECT_SOURCE_DIR}/src/components/downloader.cpp
    ${PROJECT_SOURCE_DIR}/src/string.cpp)

message_colored(STATUS "Bookwyrm install directory: ${CMAKE_INSTALL_PREFIX}/bin" 33)
install(TARGETS ${PROJECT_NAME} DESTINATION bin COMPONENT binaries)

message_colored(STATUS "Bookwyrm system plugin directory: ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/plugins" 33)
install(FILES plugins/libgen.py DESTINATION share/${PROJECT_NAME}/plugins COMPONENT config)

target_include_directories(${PROJECT_NAME} BEFORE PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/lib/fmt
    PRIVATE ${CURL_INCLUDE_DIRS})

# Some pre-compile tasks:
execute_process(COMMAND git describe --tags --dirty=-git
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  OUTPUT_VARIABLE PROG_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

configure_file(
  ${PROJECT_SOURCE_DIR}/src/version.hpp.cmake
  ${CMAKE_SOURCE_DIR}/src/version.hpp
  ESCAPE_QUOTES @ONLY)

configure_file(
  ${PROJECT_SOURCE_DIR}/src/prefix.hpp.cmake
  ${CMAKE_SOURCE_DIR}/src/prefix.hpp
  ESCAPE_QUOTES @ONLY)

#if(NOT ENV{PROG_VERSION} AND NOT ENV{GIT_TAG})
#    set(errmsg "Neither $PROG_VERSION or $GIT_TAG defined; version undefined!")
#    if (BUILDING_RELEASE)
#        message_colored(FATAL_ERROR "${errmsg} Version must be defined in release!" 31)
#    else()
#        message_colored(WARNING "${errmsg}" 33)
#    endif()
#endif()

target_link_libraries(${PROJECT_NAME}
    fmt
    stdc++fs
    ${CURL_LIBRARIES}
    ${PROJECT_NAME}-core
    ${PROJECT_NAME}-tui)

add_subdirectory(core)
add_subdirectory(tui)
