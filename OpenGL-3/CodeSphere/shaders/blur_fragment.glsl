#version 330

// A uniform
uniform bool vertical;

// A sampler for sampling the original image to blur
uniform sampler2D sampler;
uniform sampler2D depthSampler;

// The output color
out vec4 fragColor;

void main()
{
    // Perform a Gaussian blur
    // Divide the current pixel by the width of the application (can be set in an uniform, maybe)
    vec2 pos = gl_FragCoord.xy/400;
    vec2 blurPos[14];

    // Store texture positions slightly off in horizontal of vertical directions
    if (vertical) {
        blurPos[ 0] = pos + vec2(0.0,-0.028);
        blurPos[ 1] = pos + vec2(0.0,-0.024);
        blurPos[ 2] = pos + vec2(0.0,-0.020);
        blurPos[ 3] = pos + vec2(0.0,-0.016);
        blurPos[ 4] = pos + vec2(0.0,-0.012);
        blurPos[ 5] = pos + vec2(0.0,-0.008);
        blurPos[ 6] = pos + vec2(0.0,-0.004);
        blurPos[ 7] = pos + vec2(0.0, 0.004);
        blurPos[ 8] = pos + vec2(0.0, 0.008);
        blurPos[ 9] = pos + vec2(0.0, 0.012);
        blurPos[10] = pos + vec2(0.0, 0.016);
        blurPos[11] = pos + vec2(0.0, 0.020);
        blurPos[12] = pos + vec2(0.0, 0.024);
        blurPos[13] = pos + vec2(0.0, 0.028);
    } else {
        blurPos[ 0] = pos + vec2(-0.028, 0.0);
        blurPos[ 1] = pos + vec2(-0.024, 0.0);
        blurPos[ 2] = pos + vec2(-0.020, 0.0);
        blurPos[ 3] = pos + vec2(-0.016, 0.0);
        blurPos[ 4] = pos + vec2(-0.012, 0.0);
        blurPos[ 5] = pos + vec2(-0.008, 0.0);
        blurPos[ 6] = pos + vec2(-0.004, 0.0);
        blurPos[ 7] = pos + vec2( 0.004, 0.0);
        blurPos[ 8] = pos + vec2( 0.008, 0.0);
        blurPos[ 9] = pos + vec2( 0.012, 0.0);
        blurPos[10] = pos + vec2( 0.016, 0.0);
        blurPos[11] = pos + vec2( 0.020, 0.0);
        blurPos[12] = pos + vec2( 0.024, 0.0);
        blurPos[13] = pos + vec2( 0.028, 0.0);
    }

    // Sample all texture points in from the texture and add them together with Gaussian terms
    fragColor = vec4(0.0);
    fragColor += texture2D(sampler, blurPos[ 0])*0.0044299121055113265;
    fragColor += texture2D(sampler, blurPos[ 1])*0.00895781211794;
    fragColor += texture2D(sampler, blurPos[ 2])*0.0215963866053;
    fragColor += texture2D(sampler, blurPos[ 3])*0.0443683338718;
    fragColor += texture2D(sampler, blurPos[ 4])*0.0776744219933;
    fragColor += texture2D(sampler, blurPos[ 5])*0.115876621105;
    fragColor += texture2D(sampler, blurPos[ 6])*0.147308056121;
    fragColor += texture2D(sampler, pos)        *0.159576912161;
    fragColor += texture2D(sampler, blurPos[ 7])*0.147308056121;
    fragColor += texture2D(sampler, blurPos[ 8])*0.115876621105;
    fragColor += texture2D(sampler, blurPos[ 9])*0.0776744219933;
    fragColor += texture2D(sampler, blurPos[10])*0.0443683338718;
    fragColor += texture2D(sampler, blurPos[11])*0.0215963866053;
    fragColor += texture2D(sampler, blurPos[12])*0.00895781211794;
    fragColor += texture2D(sampler, blurPos[13])*0.0044299121055113265;

   vec4 depth = texture(depthSampler, pos);
   // focus om something
   if(depth.x  < 0.998 )fragColor = texture2D(sampler, pos);
}
