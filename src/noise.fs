#version 330

precision highp float;

uniform sampler2D texture0;
uniform vec2 u_pixelScale;
in vec2 fragTexCoord;
out vec4 fragColor;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 st){
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = rand(i);
    float b = rand(i + vec2(1.0,0.0));
    float c = rand(i + vec2(0.0,1.0));
    float d = rand(i + vec2(1.0,1.0));

    vec2 u = f*f*(3.0-2.0*f);
    return mix(a,b,u.x) + (c-a)*u.y*(1.0-u.x) + (d-b)*u.x*u.y;
}

void main()
{
    vec4 color = texture(texture0, fragTexCoord);

    vec2 noiseCoord = fragTexCoord * u_pixelScale * 16.0;
    float n = (noise(noiseCoord) - 0.5) * 0.1;

    color.rgb += n;

    fragColor = color;
}

