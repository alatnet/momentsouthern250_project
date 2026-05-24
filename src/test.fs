#version 460 core

in vec3 color;
in vec3 fNorm;
in vec2 fUV;

out vec4 finalColor;

void main(){
    float l = dot(fNorm, normalize(vec3(1.0f, 1.0f, 1.0f)));

    l += fUV.x * 0.0001;
    l = l * 0.5 + 0.5;

    finalColor = vec4(color, 1.0f) + (0.0000001 * l);
}