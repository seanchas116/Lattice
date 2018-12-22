#version 330

const float M_PI = 3.1415926535897932384626433832795;

uniform float width;
uniform vec2 viewportSize;

layout(points) in;
layout(triangle_strip, max_vertices = 16) out;

void emitForAngle(vec2 center, float depth, float angle) {
    vec2 direction = vec2(cos(angle), sin(angle));
    vec2 pos = direction * (width * 0.5) + center;
    gl_Position = vec4(pos / (viewportSize * 0.5) - 1.0, depth, 1);
    EmitVertex();
}

void main(void) {
    // TODO: do culling for points outside frustum
    vec4 center_clipSpace = gl_in[0].gl_Position;
    vec2 center = (center_clipSpace.xy / center_clipSpace.w + 1.0) * (0.5 * viewportSize);
    float depth = center_clipSpace.z / center_clipSpace.w;

    emitForAngle(center, depth, 0);

    for (int i = 1; i < 8; ++i) {
        float angle = i * (M_PI / 8.0);
        emitForAngle(center, depth, angle);
        emitForAngle(center, depth, -angle);
    }

    emitForAngle(center, depth, M_PI);

    EndPrimitive();
}
