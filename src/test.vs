#version 460 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNorm;
layout(location = 2) in vec2 vUV;

/*layout(location = 3) in ivec4 vBoneIndex;
layout(location = 4) in vec4 vBoneWeight;*/

layout(location = 3) in uint vBoneIndex[4];
layout(location = 4) in float vBoneWeight[4];

layout (std430, binding = 0) readonly buffer bones_ssbo {
    mat4 bones[];
};

/*layout (std140, binding = 0) uniform bones_ubo {
    mat4 bones[64];
};*/

uniform vec3 vCol;
uniform mat4 mvp;
uniform mat4 geom2world;
uniform mat4 norm2world;
uniform mat4 node2world;

uniform bool isSkinned;

out vec3 color;
out vec3 fNorm;
out vec2 fUV;

void main(){
   vec3 local_pos = vPos;
   //local_pos += evaluate_blend_shape(int(a_vertex_index));
   vec3 world_pos = vec3(0.0f);
   vec3 world_normal = vec3(0.0f);

   if (isSkinned){
      //do bone calcs here
      /*mat4 bind_to_world
        = bones[vBoneIndex.x] * vBoneWeight.x
        + bones[vBoneIndex.y] * vBoneWeight.y
        + bones[vBoneIndex.z] * vBoneWeight.z
        + bones[vBoneIndex.w] * vBoneWeight.w;*/

      mat4 bind_to_world
        = bones[vBoneIndex[0]] * vBoneWeight[0]
        + bones[vBoneIndex[1]] * vBoneWeight[1]
        + bones[vBoneIndex[2]] * vBoneWeight[2]
        + bones[vBoneIndex[3]] * vBoneWeight[3];
      world_pos = (bind_to_world * vec4(local_pos, 1.0f)).xyz;
      world_normal = (bind_to_world * vec4(vNorm, 0.0f)).xyz;
   } else {
      world_pos = (geom2world * vec4(vPos, 1.0f)).xyz;
      world_normal = (norm2world * vec4(vNorm, 0.0f)).xyz;
   }

   gl_Position = mvp * vec4(world_pos, 1.0f);
   fNorm = normalize(world_normal);
   fUV = vUV;

   color = vec3(0.0f);

   if (vBoneIndex[0] == 0) color = vec3(1.0f, 0.0f, 1.0f);
   if (vBoneIndex[0] == 1) color = vec3(1.0f, 1.0f, 0.0f);
}