#version 330

uniform float width;
uniform vec2 viewportSize;

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

void main(void) {
    vec4 p0_clipSpace = gl_in[0].gl_Position;
    vec4 p1_clipSpace = gl_in[1].gl_Position;

    vec2 p0 = (p0_clipSpace.xy / p0_clipSpace.w + 1.0) * (0.5 * viewportSize);
    vec2 p1 = (p1_clipSpace.xy / p1_clipSpace.w + 1.0) * (0.5 * viewportSize);

    vec2 direction = p1 - p0;
    vec2 offset = normalize(vec2(-direction.y, direction.x)) * (width * 0.5);
    vec2 offset_normalized = offset / viewportSize * 2.0;

    gl_Position = vec4(offset_normalized * p0_clipSpace.w, 0, 0) + p0_clipSpace;
    EmitVertex();
    gl_Position = vec4(-offset_normalized * p0_clipSpace.w, 0, 0) + p0_clipSpace;
    EmitVertex();
    gl_Position = vec4(offset_normalized * p1_clipSpace.w, 0, 0) + p1_clipSpace;
    EmitVertex();
    gl_Position = vec4(-offset_normalized * p1_clipSpace.w, 0, 0) + p1_clipSpace;
    EmitVertex();

    EndPrimitive();
}
