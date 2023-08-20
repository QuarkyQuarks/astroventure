out vec3 fragColor;
in vec2 texCoord;

uniform sampler2D image;

void main() {
    fragColor = texture(image, texCoord).rgb;
}