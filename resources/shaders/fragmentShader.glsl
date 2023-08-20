#alp include <Shading/specularBlinnPhong>
#alp include <Shading/diffuseLambert>

in vec3 viewPosition;
in vec3 viewNormal;
in vec2 texCoord;

layout(location = 0) out vec4 fragColor;

uniform sampler2D diffuseTex;
uniform float shininess;
uniform float specularStrength;
uniform float diffuseStrength;
uniform float ambientStrength;

// TODO: remove as useless? It seems that lambert lighting model fits good
float getK(in vec3 normal) {
    vec3 screenSurface = normalize(vec3(normal.x, normal.y, 0));

    float cosAlpha = dot(screenSurface, normal);
    float f = 1.0f - abs(cosAlpha) * 0.98f;
    float k = pow(f, 0.25f);

    return k;
}

void main() {
    vec3 color = texture(diffuseTex, texCoord).xyz;

    //float k = getK(viewNormal + viewPosition * 0.05f);

    const vec3 lightEyePos = vec3(0.0f, 0.0f, 0.0f);
    const vec3 cameraEyePos = vec3(0.0f, 0.0f, 0.0f);

    vec3 lightDir = normalize(lightEyePos - viewPosition);

    float spec = specularBlinnPhong(shininess, viewPosition, viewNormal, lightDir);
    vec3 specularColor = specularStrength * spec * color;

    float diff = diffuseLambert(viewNormal, vec3(0.0f, 0.0f, -viewPosition.z));
    vec3 diffuseColor = diffuseStrength * diff * color;

    vec3 ambientColor = ambientStrength * color;

    //fragColor = vec4((color + specularColor) * k, 1.0f);
    fragColor = vec4(specularColor + diffuseColor + ambientColor, 1.0f);
}
