find_package(glm REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_definitions(-DSHADER_FOLDER="${PixelEngine_DIR}/shader/")

set(PE_FOUND TRUE)
set(PE_INCLUDE_DIRS ${PixelEngine_DIR}/include ${GLEW_INCLUDE_DIRS} ${glfw3_INCLUDE_DIRS})
if(EXISTS ${PixelEngine_DIR}/lib)
	link_directories(${PixelEngine_DIR}/lib)
endif()
set(PE_LIBRARIES PixelEngine glm::glm GLEW::glew glfw)