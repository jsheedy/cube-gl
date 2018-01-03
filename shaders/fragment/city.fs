#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec4 fragPos;

flat in int instanceID;

uniform int selected;

uniform float t;

void main()
{
    vec3 lightDir = vec3(0.0, -1.0, 0.0);
    // vec3 lightDir = normalize(vec3(sin(t)*100.0, min(abs(cos(t)), 0.4)*100.0 , 0.0));
    float depth = 1.0 + fragPos.z / 5000.0; //1.0 - min(max(-fragPos.z / 1000.0, 1.0), 0.0) ; //gl_FragCoord.z / gl_FragCoord.w;
    float diff = max(dot(normal, lightDir), 0.0);

    // FragColor = vec4(sin(fragPos.x), cos(fragPos.y), fragPos.z, fragPos.x);
    // FragColor = vec4(fragPos.x,diff, 0, 0);
    // FragColor = vec4(
    //     diff * (sin(t)/2.0 + 0.5),
    //     diff * sin(1.1*t)/2.0 + 0.5,
    //     0.5,
    //     0
    // );
    if (instanceID == selected) {
        FragColor = vec4(1.0, 0, 0, 1.0);
    } else {
        // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        float r = diff * depth;
        float g = diff * depth;
        float b = diff * depth;
        FragColor = vec4(r, g, b, 1.0);
        // FragColor = vec4(instanceID / 100.0 * diff, diff, diff, 1.0);
    }
}