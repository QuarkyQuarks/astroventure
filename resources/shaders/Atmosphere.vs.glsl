layout(location = 0) in vec4 inPos;

out vec3 pos;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * inPos;

    pos = inPos.xyz;
}