## Grid of Life
This is an educational project with a focus on learning modern C++ and OpenGL while having fun experimenting with various techniques to improve (or atleast be aware of) performance. It involved implementing [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) in a 3D environment using OpenGL and C++. 

The application was made platform-independent by utilizing a GLFW graphics backend and a Dear ImGui user interface with a CMake build environment. The implementation tries to focus on efficiency and performance with the use of multithreading, instancing for rendering, Boost C++ libraries ([boost::unordered_flat_map](https://www.boost.org/doc/libs/1_81_0/libs/unordered/doc/html/unordered.html#unordered_flat_map)), and runtime analysis through the [Tracy Profiler](https://github.com/wolfpld/tracy). Additionally, the simulation was accelerated through the use of hardware-dependent SIMD AVX2 intrinsics or OpenCL Compute Shaders. 

Overall, this project was a great way to gain experience with modern C++ and OpenGL while exploring various performance optimization techniques.

[Demo video](https://github.com/xtxyTge2/grid_of_life)

# Features



# Building

# Third party libraries

GLM  
GLAD  
GLFW  
BOOST (only used unordered::unordered_flat_map)

# References

OpenGL  
https://learnopengl.com/  
OpenGL Programming Guide: The Official Guide to Learning Opengl, Version 4.5 with Spir-V
       
