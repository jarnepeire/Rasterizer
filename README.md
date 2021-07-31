# Software/Hardware Rasterizer
![DXRobot](/Screenshots/DXRobot_Animated.gif)

## Description
After having made a custom Ray Tracer from scratch, this Rasterizer was its follow-up on my graphics programming journey. The main focus of this project was getting familiar with the rendering pipeline and understanding the fundamental steps of it by re-creating it from scratch. Different topics compared to the Ray Tracer were introduced here.

As a follow-up, the same Rasterizer was made in the DirectX11 framework and has the functionality to toggle between the software and hardware Rasterizer. In my free time I've profiled and refactored the project, and made it possible to support multiple lights as well as a PBR implementation of the Cook-Torrance BRDF (as seen in my Ray Tracer project). Below you can find the visual difference between the Software Rasterizer (grey) versus the DirectX11 Hardware Rasterizer (blue).

![SRAS](/Screenshots/SRAS_Animated.gif) 
![DX](/Screenshots/DX_Animated.gif)

## Topics Covered
* Linear Algebra (Vector- and Matrix math)
* Coordinate Systems
* Ray Intersections
* Orthographic/Perspective Camera
* Orthonormal Base (ONB)
* Depth Buffer
* Depth Interpolation
* Textures & Vertex Attributes
* Texture Sampling & Filtering
* BRDF's (Lambert, Phong, Cook-Torrance)
* Physically Based Rendering (PBR)
* DirectX Graphics Pipeline
* OBJ Parser

Though with all the progress, this Rasterizer can definitely be expanded upon in terms of optimization or extra features such as Multithreading, Indirect Lighting, Reflections, Shadows and Anti-Aliasing to name a few.

## Most Interesting Code Snippets
Triangle Hit function (line 30)

Vertex Transformations (line 114)

[View Triangle Code](https://github.com/jarnepeire/Rasterizer/blob/main/source/Triangle.cpp)

-------------------------------------

Pixel Loop (line 304)

Pixel Shading (line 340)

[View Rendering Code](https://github.com/jarnepeire/Rasterizer/blob/main/source/ERenderer.cpp)

## Contributors
Credits to [Matthieu Delaere](https://www.linkedin.com/in/matthieu-delaere/), a lecturer at Howest DAE for writing the base files (math library timer, color structs, SDL window). 
