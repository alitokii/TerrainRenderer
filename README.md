# TerrainRenderer
A coding challenge to make a terrain renderer. I decided to use OpenGL.
This program offers two modes of terrain generation: Perlin noise and heightmap image. The rendered terrain is displayed with lighting and shading effects, and the user can interact with the scene using keyboard and mouse controls.

## Features Overview
* Generates 3D terrain using Perlin noise or heightmap images
* Renders the terrain with lighting and shading effects
* Supports toggling between wireframe rendering and surface normals rendering.
* Navigate through the terrain using keyboard controls
* Calculates and displays frame rate (FPS) in the console

## Dependencies
In order to build and run this program, you need to have the following dependencies installed:

* OpenGL 3.3 or higher
* GLEW (OpenGL Extension Wrangler Library)
* GLFW (OpenGL Framework)
* GLM (OpenGL Mathematics)
* stb_image.h (Single-file image loading library)
* cmake (for building the project)

For compiling the program, you need to have a C++ compiler that supports C++11 standard.
Keep in mind I am using an Ubuntu 22.04 based system, so these instructions are for linux.
Please refer to https://vulkan.lunarg.com/ for other operating systems.
Following the official Vulkan SDK installation for linux, run the following command to install the development environment with this command:

`sudo apt install libglfw3-dev libglew-dev libglm-dev`

## Performance warnings
Depends on your system configurations and if you are using a gpu or internal graphics.
Depends on size of the image (i.e. 1080p vs 2k)
I am using a internal AMD Radeon graphics, which has 2GB of video memory.

## Building the program
After installing all dependencies, you can build the program by navigating into the project directory and running `make` in the terminal.
This will compile the program for you and create an executable file named `TerrainRenderer`.

## Usage
You can run the program by executing the `TerrainRenderer` executable in the terminal. You will then be prompted to choose a terrain generation mode: Perlin noise(1) or default heightmap image(2), or custom heightmap image(3).

Regardless of what option you choose, you will have the ability to move around the terrain using the following keyboard controls:

* `WASD`: Move the camera forward, left, backward, and right, respectively
* `F`: Toggle wireframe rendering
* `N`: Toggle surface normals rendering
* `Mouse`: Move the camera view direction


## Graphics memory usage
The program stores the terrain geometry (vertices and indices) in GPU memory using vertex buffer objects (VBOs) and element buffer objects (EBOs).
Efficiently storing the terrain geometry in GPU memory allows the program to render large terrains with high performance.

## Rendering performance
The program performs well to render images with normals and wireframe. Turning on wireframe rendering will reduce the rendering performance, as it requires additional processing to render the wireframe on top of the terrain surface, but is not an issue for images below 2k.
The program utilizes shader-based rendering techniques to optimize performance. The vertex and fragment shaders are compiled and linked to efficiently process the terrain geometry and apply lighting and shading effects. An external GPU would perform much better than my internal graphics.

- I managed a stable 59 fps with a 2k image.

- When wireframe is on, I still managed 59 fps.

- When normals are on I fluctuated between 43-51 fps.

- With both normals and wireframe on, I managed 38-42 fps.

- In perlin noise, I kept a stable 59 fps with all options on.

Keep in mind this is all with `internal laptop graphics`, so a `dedicated` GPU would perform much better with minimal effort.
