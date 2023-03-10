cmake_minimum_required(VERSION 3.16.0)

project(grid_of_life)



set(PROJECT_LIBRARIES_DIR "${CMAKE_SOURCE_DIR}/libs")

# GLFW
add_subdirectory("${PROJECT_LIBRARIES_DIR}/vendored/glfw" EXCLUDE_FROM_ALL)

# Dear Imgui
add_library(imgui 
	"${PROJECT_LIBRARIES_DIR}/imgui/imgui.cpp"
	"${PROJECT_LIBRARIES_DIR}/imgui/imgui_demo.cpp"
	"${PROJECT_LIBRARIES_DIR}/imgui/imgui_draw.cpp"
	"${PROJECT_LIBRARIES_DIR}/imgui/imgui_tables.cpp"
	"${PROJECT_LIBRARIES_DIR}/imgui/imgui_widgets.cpp"
	"${PROJECT_LIBRARIES_DIR}/imgui/backends/imgui_impl_glfw.cpp"
	"${PROJECT_LIBRARIES_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
)
target_include_directories(
   imgui
   PUBLIC
      "${PROJECT_LIBRARIES_DIR}/imgui"
      "${PROJECT_LIBRARIES_DIR}/imgui/backends"
	  
	  
	  "${PROJECT_LIBRARIES_DIR}/vendored/glfw/include"
)

# glad - opengl loader
add_library(glad "${PROJECT_LIBRARIES_DIR}/glad/src/glad.c")
target_include_directories(glad PRIVATE "${PROJECT_LIBRARIES_DIR}/glad/include")

# - Tracy
option(TRACY_ENABLE "" ON)
add_library(tracy "${PROJECT_LIBRARIES_DIR}/tracy/public/TracyClient.cpp")
target_include_directories(tracy PRIVATE "${PROJECT_LIBRARIES_DIR}/tracy")
################################################################################
# Source groups
################################################################################


add_executable(${PROJECT_NAME}
    "${PROJECT_SOURCE_DIR}/src/camera.cpp"
    "${PROJECT_SOURCE_DIR}/src/chunk.cpp"
    "${PROJECT_SOURCE_DIR}/src/coordinate.cpp"
    "${PROJECT_SOURCE_DIR}/src/cube.cpp"
    "${PROJECT_SOURCE_DIR}/src/cube_system.cpp"
    "${PROJECT_SOURCE_DIR}/src/grid.cpp"
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/opencl_context.cpp"
    "${PROJECT_SOURCE_DIR}/src/opencl_grid.c"
    "${PROJECT_SOURCE_DIR}/src/read.cpp"
    "${PROJECT_SOURCE_DIR}/src/renderer.cpp"
    "${PROJECT_SOURCE_DIR}/src/shader.cpp"
    "${PROJECT_SOURCE_DIR}/src/state.cpp"
    "${PROJECT_SOURCE_DIR}/src/texture.cpp"
    "${PROJECT_SOURCE_DIR}/src/ui_state.cpp"
    "${PROJECT_SOURCE_DIR}/src/world.cpp"
)

################################################################################
# Target
################################################################################

target_include_directories(
   ${PROJECT_NAME}
   PUBLIC
	  "${PROJECT_LIBRARIES_DIR}/glad/include"
	  "${PROJECT_LIBRARIES_DIR}/boost_minimal"
	  "${PROJECT_LIBRARIES_DIR}/stb"
	  "${PROJECT_LIBRARIES_DIR}/tracy/public"
	  
	  "${PROJECT_LIBRARIES_DIR}/vendored/glm"
	  "${PROJECT_LIBRARIES_DIR}/vendored/glfw/include"
	PRIVATE
      ${PROJECT_SOURCE_DIR}
)

################################################################################
# Compile and link options
################################################################################

target_compile_options(${PROJECT_NAME} PRIVATE "-mavx2")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_NAME} PUBLIC   
	imgui
	glfw
	glad
	tracy
)

#copy assets into build dir
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

#copy shader source files into build dir
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders)