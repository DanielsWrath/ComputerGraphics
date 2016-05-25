#version 330
// Specify the inputs to the fragment shader
in vec3 outColor;
in vec3 normalIn;
in vec2 uv;

// Specify the Uniforms of the vertex shaders
uniform vec3 MaterialColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 pos;
uniform vec4 intensities;
uniform mat3 normalMatrix;
uniform sampler2D sampler;

// Specify the output of the fragment shader
// Usually a vec4 describing a color
out vec4 fColor;

void main()
{
    // Plain white
    vec3 normal = normalize(normalIn)*normalMatrix;
    float S = 0;
    // Calculate Light vector, view vector, R vector (opposite of L) and H vector
    vec3 lightVec = normalize(lightPos - gl_FragCoord.xyz);
    vec3 viewVec = normalize(viewPos - gl_FragCoord.xyz);
    vec3 H = (lightVec+viewVec)/length(lightVec+viewVec);
    
    // Calculate shading using phong calculations
    float A = intensities.x;
    float D = intensities.y* max(dot(normal,lightVec),0);
    S = intensities.z* pow(dot(H,normal),4 * intensities.w);
    if (dot(H,normal)<0){S = 0;}
    float I = A + D;
    vec4 tex = texture2D(sampler, uv);
    // Use tex instead of MaterialColor for textures
    fColor = vec4(MaterialColor.rgb*I + S,1);
    //fColor = tex;
}
