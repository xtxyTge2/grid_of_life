cmake_minimum_required(VERSION 3.16.0)

project(grid_of_life)


################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
)


################################################################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")


set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(PROJECT_LIBRARIES_DIR "${CMAKE_SOURCE_DIR}/3rdparty")

# GLM
set(GLM_DIR "${PROJECT_LIBRARIES_DIR}/glm")
add_subdirectory(${GLM_DIR} EXCLUDE_FROM_ALL)


# GLFW
set(GLFW_DIR "${PROJECT_LIBRARIES_DIR}/glfw")
set(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")
add_subdirectory(${GLFW_DIR} EXCLUDE_FROM_ALL)

# Dear Imgui
set(IMGUI_DIR  "${PROJECT_LIBRARIES_DIR}/imgui")
file(GLOB IMGUI_SOURCES
   "${IMGUI_DIR}/*.cpp"
   "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
   "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(
   imgui
   PUBLIC
      ${IMGUI_DIR}
      "${IMGUI_DIR}/backends"
	  ${GLFW_INCLUDE_DIR}
)


# glad - opengl loader
set(GLAD_DIR "${PROJECT_LIBRARIES_DIR}/glad")
set(GLAD_SOURCES_DIR "${GLAD_DIR}/src")
set(GLAD_INCLUDE_DIR "${GLAD_DIR}/include")
file(GLOB GLAD_SOURCES "${GLAD_SOURCES_DIR}/glad.c")
add_library(glad ${GLAD_SOURCES})
target_include_directories(glad PRIVATE ${GLAD_INCLUDE_DIR})

# boost library
set(BOOST_DIR "${PROJECT_LIBRARIES_DIR}/boost_1_81_0")

# stb: header only lib
set(STB_DIR "${PROJECT_LIBRARIES_DIR}/stb")

# - Tracy
option(TRACY_ENABLE "" ON)
option(TRACY_ON_DEMAND "" ON)
add_subdirectory("${PROJECT_LIBRARIES_DIR}/tracy")

################################################################################
# Source groups
################################################################################

add_executable(${PROJECT_NAME}
    "${PROJECT_SOURCE_DIR}/camera.cpp"
    "${PROJECT_SOURCE_DIR}/chunk.cpp"
    "${PROJECT_SOURCE_DIR}/coordinate.cpp"
    "${PROJECT_SOURCE_DIR}/cube.cpp"
    "${PROJECT_SOURCE_DIR}/cube_system.cpp"
    "${PROJECT_SOURCE_DIR}/grid.cpp"
    "${PROJECT_SOURCE_DIR}/main.cpp"
    "${PROJECT_SOURCE_DIR}/opencl_context.cpp"
    "${PROJECT_SOURCE_DIR}/opencl_grid.c"
    "${PROJECT_SOURCE_DIR}/read.cpp"
    "${PROJECT_SOURCE_DIR}/renderer.cpp"
    "${PROJECT_SOURCE_DIR}/shader.cpp"
    "${PROJECT_SOURCE_DIR}/state.cpp"
    "${PROJECT_SOURCE_DIR}/texture.cpp"
    "${PROJECT_SOURCE_DIR}/ui_state.cpp"
    "${PROJECT_SOURCE_DIR}/world.cpp"
)

################################################################################
# Target
################################################################################

target_include_directories(
   ${PROJECT_NAME}
   PUBLIC
	  ${GLAD_INCLUDE_DIR}
	  ${BOOST_DIR}
	  ${GLM_DIR}
	  ${STB_DIR}
	PRIVATE
      ${PROJECT_SOURCE_DIR}
)

#if(MSVC OR CLANG)
#	set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY MultiThreadedDebugDLL)
#endif()

################################################################################
# Compile definitions
################################################################################


target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
    	   "_DEBUG"
    ">"
    "$<$<CONFIG:Release>:"
    	   "NDEBUG;"
		"TRACY_ENABLE"  
    ">"
)

################################################################################
# Compile and link options
################################################################################

if(MSVC OR CLANG)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /Ob2;
            /Oi;
            /GT;
            /arch:AVX2;
            /Oy;
            /Gy;
            /fp:fast;
			/Qpar
			/Qpar-report:2
			/openmp;
			/Ox
			/O3
        >
		-mavx2
        /permissive-;
        /sdl;
        /W3;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF
        >
        /SUBSYSTEM:CONSOLE
    )
endif()


################################################################################
# Dependencies
################################################################################

target_link_libraries(${PROJECT_NAME} PUBLIC 
	libomp 
	glfw 
	imgui
	glad
	Tracy::TracyClient
)


# install instructions -> copy data and shader files into build folder
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/dependencies)