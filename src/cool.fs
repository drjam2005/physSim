#version 330

uniform sampler2D texture0;
uniform vec2 u_mouse;
uniform float u_radius;
in vec2 fragTexCoord;
out vec4 fragColor;

void main() {
    vec4 orig = texture(texture0, fragTexCoord);

    float dist = length(fragTexCoord - u_mouse);
    float factor = clamp(dist / u_radius, 0.0, 1.0);
    fragColor = vec4(mix(orig.rgb, vec3(0.0), factor), orig.a);
}

