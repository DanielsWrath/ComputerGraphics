#version 330
// Specify the inputs to the fragment shader
in vec3 outColor;

// Specify the Uniforms of the vertex shaders

// Specify the output of the fragment shader
// Usually a vec4 describing a color
out vec4 fColor;

void main()
{
    // Plain white
    fColor = vec4(outColor,1);
}
