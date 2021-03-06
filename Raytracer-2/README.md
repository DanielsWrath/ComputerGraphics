# Raytracer-2 assignment for computer graphics
=====================================================
Authors: Daniël Haitink and Robin Entjes

The map code contains all needed files. In code/scenefiles there are yaml scenes and images generated by the program.

Create the program by running: make ./ray
Then run: ./ray senefiles/"filename".yaml
Alternatively, ray.sh can be run, which generates the program and creates all the images (takes a while).

Changes
=====================================================
Raytracer.cpp/.h
Added code which allows more yaml input. Added support for camera (eye, center, up, viewSize), shadows, recursion depth and supersampling. If it is given, it will be parsed and it will be used in scene.cpp/.h

Scene.cpp/.h
ObjectTrace is added, which is used for shadows. It uses a ray and checks if it intersects with some object, if so it returns the object.

In trace, a recursive int is added. This is used for reflections. If the max recursion depth is reached, it returns black. 
It now also supports shadows, for which objectTrace is used. If a shadow is found, D and S are set to 0.
The shading is updated so it is more correct.
At the end, the final color is increased by a recursive call, using the reflection of V.

In render it now can calculate the camera model, if used. This is done using the up, eye and center vectors. This is then used instead of the eye vector.

In the trace loop (x and y) two new loops are added (ssX and ssY) which are used for supersampling. Using these loops, the offset is calculated (ssAdd * ssX or ssY) which is then added to the pixel. The colors returned by trace are all added and divided by supesampling^2, and then clamped, resulting in a less edgy and rough image.

Furthermore, some setters and getters are added.
