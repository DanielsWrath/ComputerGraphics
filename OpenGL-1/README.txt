OpenGL1 assignment for "Introduction to Computer Graphics" 2015/2016
===============================================================
Authors: Daniel Haitink and Robin Entjes.

The map Code holds the needed files to show a coloured cube on the screen which can be manipulated using mouse input.
The map CodeSphere holds the needed files to show the scene from Ratracer 1, which can also be manipulated using mouse input.

We used openGL 3.3 Core functions instead of Qt wrappers, this may cause problems on LWP PC's!
The functions are accessed using m_funcs, this is done in openGLwindow.cpp/.h

In both maps, we added code to compute the projection matrix, the model matrix and the view matrix. The variables are also set in the start. We implemented basic openGL functions which generate the Vertex Array and color or normal bufferarrays. This is then stored in a GLuint. The arrays are filled with the corresponding information and then set to the right location (0 for vertex, 1 for colors, 2 for normals).

In the cube files we added code to compute 12 random colours, using a number as seed, which is then multiplied by the iterations in the for loops. Then this color is added three times to an array, which is then put in the buffer array.

The cube code then streams the uniform variables and calls draw which will generate the frame.

The Sphere code first needs to perform renderSphere per sphere, and per sphere all the uniforms needed are streamed. The model matrix is adjusted so the points are showed correctly. In the fragmentshader the shading is calculated using the Phong calculations.

The user can click and drag on the openGL content to rotate it. This is recognized by Qt, and then we compute the difference in position of the mouse. This is used to calculate the rotation. We also change the FoV if the scrollwheel is used within the openGL context.