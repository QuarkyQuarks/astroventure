layout(location = 0) out vec4 fragColor;

in vec3 pos;

uniform vec3 color;

void main() {
    float l = length(pos);

    if (l <= 1.0f) {
        fragColor = vec4(color, smoothstep(1.0f, 0.4f, l));
    } else {
        discard;
    }
}