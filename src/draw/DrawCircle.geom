#version 330

const float M_PI = 3.1415926535897932384626433832795;

uniform vec2 viewportSize;
uniform float zOffset;

layout(points) in;
layout(triangle_strip, max_vertices = 16) out;

in vec4 vertexColor_vert[];
in float width_vert[];
out vec4 vertexColor_geom;

void emitForAngle(vec2 center, float depth, float angle) {
    vec2 direction = vec2(cos(angle), sin(angle));
    vec2 pos = direction * (width_vert[0] * 0.5) + center;
    gl_Position = vec4(pos / (viewportSize * 0.5) - 1.0, depth, 1);
    vertexColor_geom = vertexColor_vert[0];
    EmitVertex();
}

void main(void) {
    // TODO: do culling for points outside frustum
    vec4 center_clipSpace = gl_in[0].gl_Position;
    vec2 center = (center_clipSpace.xy / center_clipSpace.w + 1.0) * (0.5 * viewportSize);
    float depth = center_clipSpace.z / center_clipSpace.w + zOffset;

    emitForAngle(center, depth, 0);

    for (int i = 1; i < 8; ++i) {
        float angle = i * (M_PI / 8.0);
        emitForAngle(center, depth, angle);
        emitForAngle(center, depth, -angle);
    }

    emitForAngle(center, depth, M_PI);

    EndPrimitive();
}
