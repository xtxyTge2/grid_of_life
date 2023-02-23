cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(sancious_shaders C CXX)

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
)

################################################################################
# Global compiler options
################################################################################

set(CMAKE_C_FLAGS "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")

################################################################################
# Source groups
################################################################################
set(Header_Files
    "3rdparty/imgui/imconfig.h"
    "3rdparty/imgui/imgui.h"
    "3rdparty/imgui/imgui_impl_glfw.h"
    "3rdparty/imgui/imgui_impl_opengl3.h"
    "3rdparty/imgui/imgui_impl_opengl3_loader.h"
    "3rdparty/imgui/imgui_internal.h"
    "3rdparty/imgui/imstb_rectpack.h"
    "3rdparty/imgui/imstb_textedit.h"
    "3rdparty/imgui/imstb_truetype.h"
    "src/camera.hpp"
    "src/chunk.hpp"
    "src/coordinate.hpp"
    "src/cube.hpp"
    "src/cube_system.hpp"
    "src/grid.hpp"
    "src/opencl_context.hpp"
    "src/opengl.hpp"
    "src/read.hpp"
    "src/renderer.hpp"
    "src/shader.hpp"
    "src/shader_program.hpp"
    "src/state.hpp"
    "src/stb_image.h"
    "src/texture.hpp"
    "src/ui_state.hpp"
    "src/world.hpp"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "3rdparty/imgui/imgui.cpp"
    "3rdparty/imgui/imgui_demo.cpp"
    "3rdparty/imgui/imgui_draw.cpp"
    "3rdparty/imgui/imgui_impl_glfw.cpp"
    "3rdparty/imgui/imgui_impl_opengl3.cpp"
    "3rdparty/imgui/imgui_tables.cpp"
    "3rdparty/imgui/imgui_widgets.cpp"
	"3rdparty/tracy/TracyClient.cpp"
    "src/camera.cpp"
    "src/chunk.cpp"
    "src/coordinate.cpp"
    "src/cube.cpp"
    "src/cube_system.cpp"
    "src/glad.c"
    "src/grid.cpp"
    "src/main.cpp"
    "src/opencl_context.cpp"
    "src/opencl_grid.c"
    "src/read.cpp"
    "src/renderer.cpp"
    "src/shader.cpp"
    "src/shader_program.cpp"
    "src/state.cpp"
    "src/texture.cpp"
    "src/ui_state.cpp"
    "src/world.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

set(ROOT_NAMESPACE sanciousshaders)

target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/tracy/tracy)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/moodycamel)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/imgui)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/glm)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/glfw-3.3.8/include)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/boost_1_81_0)


################################################################################
# MSVC runtime library
################################################################################

if(MSVC OR CLANG)
	set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY MultiThreadedDebugDLL)
endif()

################################################################################
# Compile definitions
################################################################################

if(MSVC OR CLANG)
	target_compile_definitions(${PROJECT_NAME} PRIVATE
    	"$<$<CONFIG:Debug>:"
    	    "_DEBUG"
    	">"
    	"$<$<CONFIG:Release>:"
    	    "NDEBUG;"
			"TRACY_ENABLE"
    	    ""
    	">"
	)
endif()

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
            /Ot;
            /fp:fast
        >
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

if(WIN32)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "libomp"
        "glfw3dll"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if(WIN32)
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glfw-3.3.8/lib-vc2022"
    )
endif()

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/dependencies)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glfw-3.3.8/lib-vc2022/glfw3.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)