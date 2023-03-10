## Grid of Life ![example workflow](https://github.com/xtxyTge2/grid_of_life/actions/workflows/build.yml/badge.svg)
This is an educational project with a focus on learning modern C++ and OpenGL while having fun experimenting with various techniques to improve (or atleast be aware of) performance. It involved implementing [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) in a 3D environment. 

Game of Life was choosed because on the one hand it is very easy to implement and on the other hand scaling up the simulation can pose many challenges and design questions. A naive implementation can be implemented very quickly, which allows for exploration and experimentation with many different techniques, while not being hindered by a overly complex simulation/problem. In other words it is simple enough as to be able to quickly iterate and focus on learning, which after all was the main goal.

The application was made platform-independent by utilizing a GLFW graphics backend and a Dear ImGui user interface with a CMake build environment. The implementation tries to focus on efficiency and performance with the use of multithreading, instancing for rendering, Boost C++ libraries ([boost::unordered_flat_map](https://www.boost.org/doc/libs/1_81_0/libs/unordered/doc/html/unordered.html#unordered_flat_map)), and runtime analysis through the use of the [Tracy Profiler](https://github.com/wolfpld/tracy). Additionally, the simulation was accelerated through the use of handwritten hardware-dependent SIMD AVX2 intrinsics and at some point OpenCL-compute shaders.

Overall, this project was a great way to gain experience with modern C++ and OpenGL, while exploring various performance optimization techniques.

[Demo video](https://github.com/xtxyTge2/grid_of_life)

# Building

To build the project we need CMake.

```
git clone https://github.com/xtxyTge2/grid_of_life
cd grid_of_life
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
Configure CMake in a 'build' subdirectory. `CMAKE_BUILD_TYPE` is only required if you are using a single-configuration generator such as make. See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html?highlight=cmake_build_type

# Fresh Ubuntu setup before building the project
(For my own documentation we record the steps for setting up a fresh ubuntu install ready to build the project, ie install build-tools, cmake and all the dependencies of the GLFW library.) 

1. Install gcc, build-tools, libgl1-mesa-dev (for opengl) and cmake.
```
sudo apt-get update
sudo apt-get install -y build-essential libgl1-mesa-dev cmake
```

Now the glfw library depends on a few libraries itself.  
2a. Install glfw library dependencies for X11  
```
sudo apt install -y libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxext-dev
 ```
2b. If the system uses X11+Wayland additionally install the following libraries  
```
sudo apt install -y wayland-protocols libwayland-dev libxkbcommon-dev
```
For a reference see the build script of the glfw library https://github.com/glfw/glfw/blob/master/.github/workflows/build.yml.  

3. Follow the build instructions above.


# Third party libraries/tools
- [GLM](https://github.com/g-truc/glm)  
- [GLAD](https://glad.dav1d.de/)  
- [GLFW](https://github.com/glfw/glfw)   
- [BOOST](https://www.boost.org/) (only using unordered::unordered_flat_map and unordered::unordered_flat_set and their minimal dependencies)  
- [Tracy](https://github.com/wolfpld/tracy)  
- [Dear Imgui](https://github.com/ocornut/imgui)  

# References/Resources
Some references and resources, which were useful to me.

Tutorials/Blogs  
- [https://learnopengl.com/](https://learnopengl.com/)  
- [https://en.algorithmica.org/](https://en.algorithmica.org/)  
- [Effective Modern CMake](https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1)  

Books  
- OpenGL Programming Guide: The Official Guide to Learning OpenGL®, Version 4.5 with SPIR-V, Ninth Edition by John Kessenich, Graham Sellers, Dave Shreiner
- C++ Concurrency in Action - Practical Multithreading by Anthony Williams  
- A Tour of C++ by Bjarne Stroustrup   

Talks/Lectures
- [Computer Enhance](https://www.computerenhance.com/) - Performance-Aware Programming Series by Casey Muratori  
- [CppCon 2014: Mike Acton "Data-Oriented Design and C++"](https://www.youtube.com/watch?v=rX0ItVEVjHc)  
- [CppCon 2014: Chandler Carruth "Efficiency with Algorithms, Performance with Data Structures"](https://www.youtube.com/watch?v=fHNmRkzxHWs)  
- [CppCon 2017: Carl Cook “When a Microsecond Is an Eternity: High Performance Trading Systems in C++”](https://www.youtube.com/watch?v=NH1Tta7purM)  
- [Computer Graphics (CMU 15-462/662)](https://www.youtube.com/watch?v=W6yEALqsD7k&list=PL9_jI1bdZmz2emSh0UQ5iOdT2xRHFHL7E)
- [Introduction to Computer Graphics](https://www.youtube.com/watch?v=vLSphLtKQ0o&list=PLplnkTzzqsZTfYh4UbhLGpI5kGd5oW_Hh&index=1)



       
