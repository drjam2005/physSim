#version 330

precision highp float;

uniform sampler2D texture0; // the particle layer
uniform float u_time;       // time in seconds
in vec2 fragTexCoord;
out vec4 fragColor;

// ====== RGB <-> HSV conversion ======
vec3 rgb2hsv(vec3 c){
    float cmax = max(c.r, max(c.g, c.b));
    float cmin = min(c.r, min(c.g, c.b));
    float delta = cmax - cmin;

    float h = 0.0;
    if(delta > 0.0){
        if(cmax == c.r) h = mod((c.g - c.b)/delta, 6.0);
        else if(cmax == c.g) h = ((c.b - c.r)/delta + 2.0);
        else h = ((c.r - c.g)/delta + 4.0);
        h /= 6.0; // normalize to [0,1]
        if(h < 0.0) h += 1.0;
    }

    float s = (cmax == 0.0) ? 0.0 : delta / cmax;
    float v = cmax;
    return vec3(h,s,v);
}

vec3 hsv2rgb(vec3 c){
    float h = c.x * 6.0;
    float s = c.y;
    float v = c.z;

    float i = floor(h);
    float f = h - i;
    float p = v * (1.0 - s);
    float q = v * (1.0 - s*f);
    float t = v * (1.0 - s*(1.0 - f));

    if(i == 0.0) return vec3(v,t,p);
    else if(i == 1.0) return vec3(q,v,p);
    else if(i == 2.0) return vec3(p,v,t);
    else if(i == 3.0) return vec3(p,q,v);
    else if(i == 4.0) return vec3(t,p,v);
    else return vec3(v,p,q);
}

void main()
{
    vec4 color = texture(texture0, fragTexCoord);

    vec3 hsv = rgb2hsv(color.rgb);

    // Shift hue over time
    hsv.x += u_time * 0.05; // speed of hue shift
    hsv.x = fract(hsv.x);    // wrap around 0..1

    color.rgb = hsv2rgb(hsv);

    fragColor = color;
}
