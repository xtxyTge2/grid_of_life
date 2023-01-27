# grid_of_life
A project aiming to replicate Conway's Game of Life in a simple 3D-World written in C++, using OpenGL for rendering.

The aim of this project is to have a hands-on project, with enough scope to learn to write C++. A particular focus is applied to making sure that the simulation/program runs fast. In parallel we learn to apply common data layout and optimization techniques.


We implemented the simulation and added a simple UI for controlling it in the application using the Dear-ImGui library. We also have simple camera movement. 

#Feature Roadmap:
	- add mouse navigation to the camera, add more camera controls (speed/reset/focus on grid etc).
	- chunk system: instead of one giant matrix, which represents the grid, we split the grid up into smaller matrices/chunks and operate on them individually and then sync them up on their borders


