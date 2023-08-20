#alp include <ToneMapping>

in vec2 texCoord;

layout (location = 0) out vec3 fragColor;

uniform sampler2D base;
uniform sampler2D blur;
uniform float k;

void main() {
    vec3 baseColor = texture(base, texCoord).rgb;
    vec3 blurColor = texture(blur, texCoord).rgb;
    vec3 blurColorTonemapped = tonemapAces(tonemapUnreal(blurColor * 0.2f));
    fragColor = mix(baseColor, blurColorTonemapped, k);
}
