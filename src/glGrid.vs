#version 460 core

layout(location = 0) in vec3 vPos;

uniform mat4 mvp;
uniform vec3 col;

out vec3 color;

void main(){
   color = col;
   gl_Position = mvp * vec4(vPos, 1.0f);
}