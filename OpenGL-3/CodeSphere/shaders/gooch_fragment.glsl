#version 330
// Specify the inputs to the fragment shader
in vec3 outColor;
in vec3 normalIn;
in vec2 uv;
in mat4 MVP;

// Specify the Uniforms of the vertex shaders
uniform vec3 MaterialColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 pos;
uniform vec4 intensities;
uniform mat3 normalMatrix;
uniform mat3 normalMatrixLine;
uniform sampler2D sampler;

// Specify the output of the fragment shader
// Usually a vec4 describing a color
out vec4 fColor;

void main()
{
    // Threshold for black lines
    float theta = -0.35;


    vec3 normal = normalize(normalIn*normalMatrix);
    float S = 0;
    // Calculate Light vector, view vector, R vector (opposite of L) and H vector
    vec3 lightVec = normalize(lightPos - gl_FragCoord.xyz);
    vec3 viewVec = normalize(viewPos - gl_FragCoord.xyz);
    vec3 H = (lightVec+viewVec)/length(lightVec+viewVec);
    
    // Phong specular
    float A = intensities.x;
    float D = intensities.y* max(dot(normal,lightVec),0);
    S = intensities.z* pow(dot(H,normal),8 * intensities.w);
    if (dot(H,normal)<0){S = 0;}

    vec4 tex = vec4(MaterialColor,1);

    // Gooch Shading
    float b = 0.55;
    float y = 0.3;
    float alpha = 0.25;
    float beta = 0.5;

    vec3 kd = tex.rgb*intensities.y;
    vec3 kCool = vec3(0,0,b)+alpha*kd;
    vec3 kWarm = vec3(y,y,0) + beta*kd;
    vec3 I = kCool *(1 - dot(lightVec, normal))/2 + kWarm * (1 + dot(lightVec, normal))/2;

    //Set tex to sampler2D(sampler, uv) if textures are needed
    fColor = vec4(A * tex.rgb + I + S, 1);
    // if border, return black
    //vec3 V = (MVP * vec4(0, 0, 1 ,1));
    vec3 N = normalize(normalIn*normalMatrixLine);
    if(!(dot(N, vec3(0, 0, -1)) < theta)) {
        fColor = vec4(0);
    }
}
