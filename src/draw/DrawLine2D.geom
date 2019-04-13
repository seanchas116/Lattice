#version 330

uniform float width;
uniform vec2 viewportSize;

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 vertexColor_vert[];
out vec4 vertexColor_geom;

void main(void) {
    vec4 p0_clipSpace = gl_in[0].gl_Position;
    vec4 p1_clipSpace = gl_in[1].gl_Position;
    vec3 p0 = vec3((p0_clipSpace.xy + 1.0) * (viewportSize * 0.5), p0_clipSpace.z);
    vec3 p1 = vec3((p1_clipSpace.xy + 1.0) * (viewportSize * 0.5), p1_clipSpace.z);

    vec4 color0 = vertexColor_vert[0];
    vec4 color1 = vertexColor_vert[1];

    vec2 direction = p1.xy - p0.xy;
    vec2 offset = normalize(vec2(-direction.y, direction.x)) * (width * 0.5);

    gl_Position = vec4((p0.xy + offset) / (viewportSize * 0.5) - 1.0, p0.z, 1);
    vertexColor_geom = color0;
    EmitVertex();

    gl_Position = vec4((p0.xy - offset) / (viewportSize * 0.5) - 1.0, p0.z, 1);
    vertexColor_geom = color0;
    EmitVertex();

    gl_Position = vec4((p1.xy + offset) / (viewportSize * 0.5) - 1.0, p1.z, 1);
    vertexColor_geom = color1;
    EmitVertex();

    gl_Position = vec4((p1.xy - offset) / (viewportSize * 0.5) - 1.0, p1.z, 1);
    vertexColor_geom = color1;
    EmitVertex();

    EndPrimitive();
}
