#version 330 core
// out vec4 FragColor;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

// in vec4 gl_FragCoord;
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;

in vec4 fragPos;
in vec3 Normal;

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 viewPos;
uniform float t;

void main()
{
    PointLight light = pointLights[0];
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - vec3(fragPos));
    float diff = max(dot(normal, lightDir), 0.0);

    float t1 = sin(t+t) / 2.0f + 1.0f;
    float t2 = sin(t*1.3f+t) / 2.0f + 1.0f;
    float t3 = sin(t*2.1f+t) / 2.0f + 1.0f;

    FragColor = vec4(
        diff + sin(fragPos.x*8 + 2*t1),
        diff + sin(fragPos.y*6 + 3*t2),
        diff + sin(fragPos.z*2 + 5*t3),
        1.0);

    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}