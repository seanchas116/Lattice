#version 330

uniform vec4 color;
uniform bool useVertexColor;

in vec4 vertexColor;

out vec4 fragColor;

void main(void) {
    fragColor = useVertexColor ? vertexColor : color;
}
