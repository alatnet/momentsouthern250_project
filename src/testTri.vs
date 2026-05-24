#version 460 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vCol;
uniform mat4 mvp;

out vec3 color;

void main(){
   color = vCol;
   gl_Position = mvp * vec4(vPos, 1.0f);
}