#version 330

in vec3 vertexColor_geom;
out vec4 fragColor;
uniform vec3 color;
uniform bool useVertexColor;

void main(void) {
    fragColor = vec4(useVertexColor ? vertexColor_geom : color, 1);
}
