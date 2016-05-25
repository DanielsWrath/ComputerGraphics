#version 330
// Minimalistic vertex shader
layout(location=0) in vec3 posAttr;

void main()
{
    gl_Position = vec4(posAttr,1);
}
