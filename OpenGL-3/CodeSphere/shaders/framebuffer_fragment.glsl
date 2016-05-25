#version 330
uniform vec2 Texcoord;
uniform sampler2D texFramebuffer;

out vec4 fragColor;
void main()
{
    fragColor = texture(texFramebuffer,gl_FragCoord.xy/ Texcoord ).rgba;
    //fragColor = vec4(1,0,1,1);
}
