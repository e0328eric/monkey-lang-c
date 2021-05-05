set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.hh *.cc)

set(SOURCES ${SOURCES})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ..)

add_executable(${BINARY} ${SOURCES})
target_link_libraries(${BINARY} readline)

add_library(${BINARY}_lib STATIC ${SOURCES})
