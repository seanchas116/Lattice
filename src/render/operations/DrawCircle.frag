#version 330

in vec4 vertexColor_geom;
out vec4 fragColor;
uniform vec4 color;
uniform bool useVertexColor;

void main(void) {
    fragColor = useVertexColor ? vertexColor_geom : color;
}
