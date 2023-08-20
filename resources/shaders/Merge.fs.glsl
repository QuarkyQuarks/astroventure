#alp include <ToneMapping> // TODO: all-in-one inclusion just for test; use ToneMapping/function in release
#alp include <Blending/screen>

out vec3 fragColor;
in vec2 texCoord;

uniform sampler2D image;
uniform sampler2D bloomImage;
uniform float gamma;

void main() {
    vec3 color = blendScreen(texture(image, texCoord).rgb, texture(bloomImage, texCoord).rgb);
    fragColor = tonemapUchimura(color);
    fragColor = pow(fragColor, vec3(1.0f / gamma)); // gamma correction
}
