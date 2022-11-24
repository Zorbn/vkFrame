#version 450

// layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragTexCoord;

layout(location = 0) out vec4 outColor;

float near = 0.1;
float far = 100.0;

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    float depth = linearizeDepth(gl_FragCoord.z);
    outColor = vec4(1.0, 0.0, 0.0, 1.0); //vec4(vec3(depth), 1.0);
    // outColor = texture(texSampler, fragTexCoord);
}
