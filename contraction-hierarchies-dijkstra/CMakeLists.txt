cmake_minimum_required(VERSION 3.3)

project(Dijkstra)

# Project: Variablen
file(GLOB_RECURSE source_files "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")
file(GLOB_RECURSE header_files "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h*")

# Projekt: Dateien, Pfade
add_executable(${PROJECT_NAME} ${source_files} ${header_files})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Projekt: Eigenschaften
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif()

SET(CMAKE_CXX_FLAGS "-O3")

message(STATUS "PROJECT_NAME=${PROJECT_NAME}")
message(STATUS "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${PROJECT_NAME} -graph ../graph.fmi -queries ../queries.txt -results ../results.txt
# )