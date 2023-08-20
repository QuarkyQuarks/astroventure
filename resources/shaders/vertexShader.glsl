layout(location = 0) in vec4 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out vec3 viewPosition;
out vec3 viewNormal;
out vec2 texCoord;

uniform mat4 MVPMatrix;
uniform mat4 modelView;

void main() {
    gl_Position = MVPMatrix * inPos;

    viewPosition = vec3(modelView * inPos);
    viewNormal = vec3(normalize(modelView * vec4(inNormal, 0.0)));
    texCoord = inTexCoord;
}
