cmake_minimum_required(VERSION 3.10)

project(PixelEngine)

if(WIN32)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
include(${CMAKE_SOURCE_DIR}/cmake/FindGlew.cmake)
find_package(glfw3 REQUIRED)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_definitions(-DSHADER_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/shader/")

add_library(PixelEngine SHARED include/PixelEngine.h src/PixelEngine.cpp)

target_include_directories(PixelEngine PRIVATE include ${GLEW_INCLUDE_DIRS} ${glfw3_INCLUDE_DIRS})
target_link_libraries(PixelEngine PRIVATE OpenGL::GL glm::glm ${GLEW_LIBRARIES} glfw)

install(TARGETS PixelEngine)
install(DIRECTORY include DESTINATION .)
install(FILES cmake/PixelEngineConfig.cmake DESTINATION .)
install(DIRECTORY shader DESTINATION .)
if(WIN32)
	install(FILES cmake/FindGlew.cmake DESTINATION cmake/FindGlew.cmake)
endif()