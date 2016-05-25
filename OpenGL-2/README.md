# openGL-2 assignment for Computer Graphics
=====================================================
Authors: Daniël Haitink and Robin Entjes

The map Code holds the needed files to show a coloured cube on the screen which can be manipulated using mouse input.
The map CodeSphere holds the needed files to show the scene from Ratracer 1, which can also be manipulated using mouse input.

We used openGL 3.3 Core functions instead of Qt wrappers, this may cause problems on LWP PC's!
The functions are accessed using m_funcs, this is done in openGLwindow.cpp/.h

======================================================

MainWindow.cpp/.h
We added code which creates a new buffer containing the UV coordinates, which are used to correctly position the textures on the objects. They are passed on to the GPU in vertex attribute array 3.
We also load textures in. In cube we only load one texture, in spheres we load in 9 (All planets and pluto, the moon and the sun). This is done in readTexture in both files (The implementation differs slightly). This then gives the texture to the GPU. 
We bind the needed textures before drawing.
In spheres, we also created a new scene, which is the solar system. We tried rotating it around the sun but this doesn't work perfectly. The balls all have a different texture and within the sun, the light source is placed.

shaders
In the shaders we added an input buffer for the UV coordinates. Also we added a sampler2D uniform, which is the texture used. Instead of using the color, we set fColor to the texture color at interpolated position UV. In spheres we also do phong shading over that. 

