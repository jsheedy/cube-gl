#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;

uniform float alpha;
uniform float t;

void main()
{
    vec3 lightDir1 = vec3(1.0, -1.0, 0.0);
    vec3 lightDir2 = vec3(1.0, 1.0, 0.0);
    // vec3 lightDir = normalize(vec3(sin(t)*100.0, min(abs(cos(t)), 0.4)*100.0 , 0.0));
    float l1 = 0.5*max(dot(normal, lightDir1), 0.0);
    float l2 = 0.5*max(dot(normal, lightDir2), 0.0);
    float diffuse = 0.1;

    float red = l1 + l2 + diffuse;
    float green = 0.1 * l1 + l2 + diffuse;
    float blue =  l1 + l2 + diffuse;
    // FragColor = vec4(sin(fragPos.x), cos(fragPos.y), fragPos.z, fragPos.x);
    // FragColor = vec4(fragPos.x,diff, 0, 0);
    // FragColor = vec4(
    //     diff * (sin(t)/2.0 + 0.5),
    //     diff * sin(1.1*t)/2.0 + 0.5,
    //     0.5,
    //     0
    // );
    FragColor = vec4(red, green, blue, alpha);
}