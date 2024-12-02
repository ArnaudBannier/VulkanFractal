#version 450

// In
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;

// Out
layout(location = 0) out vec2 outUV;

// Uniform
layout(binding = 0) uniform UniformBufferObject {
    dvec2 z0;
    dvec2 z1;
    dvec2 c;
    bool juliaSet;
    int maxIter;

    float colorExponent;
    float colorCycles;
    
    vec3 paletteBase;
    vec3 paletteAmplitude;
    vec3 paletteFrequency;
    vec3 palettePhase;
} ubo;

void main() {
    gl_Position = vec4(inPos, 1.0);
    outUV = inUV;
}