#version 330
// Specify the input locations of attributes
layout(location=0) in vec3 posAttr;     // vertex position
layout(location=1) in vec3 colAttr;     // vertex color color
layout(location=2) in vec3 normalAttr;  // vertex normal
layout(location=3) in vec2 uvAttr; // vertex texture

// Specify the Uniforms of the vertex shaders
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Specify the outputs of the vertex shader
// These will be the input for the fragment shader
out vec3 outColor;
out vec3 normalIn;
out vec2 uv;

void main()
{
    // gl_Position is the output of the vertex shader
    // Currently without any transformation
    //gl_Position = vec4(posAttr,1.0);
    outColor = colAttr;
    normalIn = normalAttr;
    uv = uvAttr;
    gl_Position = projection * view * model * vec4(posAttr,1);
}
