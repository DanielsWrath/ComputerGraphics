# OpenGL-3 assignment for computer graphics
=====================================================
Authors: Daniël Haitink and Robin Entjes

The map Code holds the needed files to show a coloured cube on the screen which can be manipulated using mouse input.
The map CodeSphere holds the needed files to show the scene from Ratracer, which can also be manipulated using mouse input.

We used openGL 3.3 Core functions instead of Qt wrappers, this may cause problems on LWP PC's!
The functions are accessed using m_funcs, this is done in openGLwindow.cpp/.h

Defines are used to render other scenes etc. Look in MainWindow.cpp/.h for the defines and what they do

======================================================
MainWindow.cpp/.h
The file is altered so it can handle multiple shaders. Also a frameBuffer is added which includes a texture (which is printed to the screen) and a depth texture, storing the depth information. This is used to create blurry images, in the render function. If blending is set, normal phong shading is used, but it is also parsed by the blur shader after this. It is also possible to blur partially, this is set in the shader. Furthermore gooch shading is added, which looks cartoonish.

Gooch Shader
The gooch shader uses the phong specular and pretty much the same vertex shader as phong. It uses the gooch shading calculations to create the diffuse color. Black lines are also added around the object, this is done by doing a dot product of the normal and eye position.

FrameBuffer Shader
The framebuffer shader is a very basic shader, which is used after phong or gooch shading. It does not alter the image, like blur does, resulting in the same image as before using framebuffers.

Blur Shader
The blur shader is performed after phong or gooch and it uses an offset with which it blurs the rendered scene. It also has a depth buffer which can be used to blur only at a certain depth.
