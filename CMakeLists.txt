cmake_minimum_required(VERSION 3.10)

project(PixelEngine)

find_package(glm REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_definitions(-DSHADER_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/shader/")

add_library(PixelEngine SHARED include/PixelEngine.h src/PixelEngine.cpp)

target_include_directories(PixelEngine PRIVATE include ${GLEW_INCLUDE_DIRS} ${glfw3_INCLUDE_DIRS})
target_link_libraries(PixelEngine PRIVATE glm::glm GLEW::glew glfw)

install(TARGETS PixelEngine)
install(DIRECTORY include DESTINATION .)
install(DIRECTORY cmake/ DESTINATION .)
install(DIRECTORY shader DESTINATION .)