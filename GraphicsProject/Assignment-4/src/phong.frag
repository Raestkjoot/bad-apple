#version 330 core

uniform vec3 AmbientLightColor;
uniform vec3 LightPosition;
uniform vec3 LightColor;

uniform vec3 EyePosition;

// front
uniform vec3  AmbientColor;
uniform vec3  DiffuseColor;
uniform vec3  SpecularColor;
// back
uniform vec3  AmbientColorBack;
uniform vec3  DiffuseColorBack;
uniform vec3  SpecularColorBack;

uniform float Shininess;

in vec3 WorldVertex;
in vec3 WorldNormal;

out vec4 FragColor;

void main() {
    vec3 color = vec3(0.0f, 0.0f, 0.0f);

    // Implement your phong shader program here.

    vec3 N = normalize(WorldNormal);
    vec3 L = normalize(LightPosition - WorldVertex);
    float NdotL = dot(N, L);

    vec3 V = normalize(EyePosition - WorldVertex);
    vec3 R = normalize(2.0f * N * NdotL - L);
    float VdotR = dot(V, R);

    // I_phong = I_ambient + I_diffuse + I_specular
    if (gl_FrontFacing) {
        color += AmbientLightColor * AmbientColor;
        color += LightColor * DiffuseColor * max(NdotL, 0.0f);
        color += LightColor * SpecularColor * pow(max(0.0f, VdotR), Shininess);
    } else {
    // flip normals or something
        color += AmbientLightColor * AmbientColorBack;
        color += LightColor * DiffuseColorBack * max((-NdotL), 0.0f);
        color += LightColor * SpecularColorBack * pow(max(0.0f, VdotR), Shininess);
    }


    FragColor = vec4(color, 1.0f);
}
