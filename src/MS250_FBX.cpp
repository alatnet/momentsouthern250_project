#include "MS250_FBX.h"
#include "MS250_Math.hpp"
#include "MS250_Structs.hpp"
#include "MS250_Utils.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <float.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <sys/types.h>

// #include "glm/ext/quaternion_geometric.hpp"
// #include "glm/fwd.hpp"
// #include "glm/matrix.hpp"
//#include "glm/gtc/type_ptr.hpp"
#include "ufbx.h"

using namespace MS250::Structs;

bool MS250_FBX::debug = false;
unsigned long MS250_FBX::MAX_BONES = 64;
unsigned long MS250_FBX::MAX_BLENDSHAPES = 64;
float MS250_FBX::ANIMATION_FRAMERATE = 30.0f;
unsigned long MS250_FBX::MAX_ANIM_FRAMES = 4096;

//-------------------------------
// Constructors
//-------------------------------
//file load
MS250_FBX::MS250_FBX(const char *filename, const ufbx_load_opts *opts, ufbx_error *error){
    this->_fbx_scene = ufbx_load_file(filename, opts, error);
    if (this->_fbx_scene != nullptr) this->_LoadData();
}

// MS250_FBX::MS250_FBX(const char *filename, size_t filename_len, const ufbx_load_opts *opts, ufbx_error *error){
//     this->_fbx_scene = ufbx_load_file_len(filename, filename_len, opts, error);
//     if (this->_fbx_scene != nullptr) this->_LoadData();
// }

// //Memory load
// MS250_FBX::MS250_FBX(const void *data, size_t size, const ufbx_load_opts *opts, ufbx_error *error){
//     this->_fbx_scene = ufbx_load_memory(data, size, opts, error);
//     if (this->_fbx_scene != nullptr) this->_LoadData();
// }

// //stdio load
// MS250_FBX::MS250_FBX(void *file, const ufbx_load_opts *opts, ufbx_error *error){
//     this->_fbx_scene = ufbx_load_stdio(file, opts, error);
//     if (this->_fbx_scene != nullptr) this->_LoadData();
// }

// MS250_FBX::MS250_FBX(void *file, const void *prefix, size_t prefix_size, const ufbx_load_opts *opts, ufbx_error *error){
//     this->_fbx_scene = ufbx_load_stdio_prefix(file, prefix, prefix_size, opts, error);
//     if (this->_fbx_scene != nullptr) this->_LoadData();
// }

// //stream load
// MS250_FBX::MS250_FBX(const ufbx_stream *stream, const ufbx_load_opts *opts, ufbx_error *error){
//     this->_fbx_scene = ufbx_load_stream(stream, opts, error);
//     if (this->_fbx_scene != nullptr) this->_LoadData();
// }

// MS250_FBX::MS250_FBX(const ufbx_stream *stream, const void *prefix, size_t prefix_size, const ufbx_load_opts *opts, ufbx_error *error){
//     this->_fbx_scene = ufbx_load_stream_prefix(stream, prefix, prefix_size, opts, error);
//     if (this->_fbx_scene != nullptr) this->_LoadData();
// }
//-------------------------------

//-------------------------------
// Deconstructor
//-------------------------------
MS250_FBX::~MS250_FBX(){

    MS250_FBX::LogNewLine("Unloading FBX.");
    if (this->_fbx_scene != nullptr) ufbx_free_scene(this->_fbx_scene);

    MS250_FBX::LogNewLine("Unloading data.");
    this->_UnloadData();
}
//-------------------------------

//-------------------------------
//-------------------------------
bool MS250_FBX::IsLoaded(){
    return this->_isLoaded;
}

void MS250_FBX::Update(float deltaTime){
    this->currAnimTime += deltaTime;
    if (this->currAnimTime >= this->_animations[this->currAnimID].time.end){
        this->currAnimTime -= this->_animations[this->currAnimID].time.end - this->_animations[this->currAnimID].time.begin;
    }
    this->UpdateAnimation(this->currAnimID, this->currAnimTime);
    this->UpdateHierarchy();
    this->UpdateBones();
}

void MS250_FBX::UpdateAnimation(size_t animID, float time){
    if (animID >= 0 && animID <= this->numAnimations){
        Animation * anim = &(this->_animations[animID]);
        float frameTime = (time - anim->time.begin) * anim->framerate;
        size_t f0 = std::min((size_t)frameTime + 0, anim->numFrames - 1);
        size_t f1 = std::min((size_t)frameTime + 1, anim->numFrames - 1);
        float t = std::min(frameTime - (float)f0, 1.0f);

        for (size_t i = 0; i < this->_numNodes; i++){
            Node * node = &(this->_nodes[i]);
            AnimNode * animNode = &(anim->nodes[i]);

            // MS250::Math::Quat rot = (animNode->rot != nullptr) ? animNode->rot[f0].lerp(animNode->rot[f1], time) : animNode->const_rot;
            // MS250::Math::Vector3 pos = (animNode->pos != nullptr) ? animNode->pos[f0].lerp(animNode->pos[f1], time) : animNode->const_pos;
            // MS250::Math::Vector3 scale = (animNode->scale != nullptr) ? animNode->scale[f0].lerp(animNode->scale[f1], time) : animNode->const_scale;

            MS250::Math::Quat rot = (animNode->rot != nullptr) ? MS250_MATH_LERP_QUAT(animNode->rot[f0], animNode->rot[f1], t) : animNode->const_rot;
            MS250::Math::Vector3 pos = (animNode->pos != nullptr) ? MS250_MATH_LERP_VECTOR3(animNode->pos[f0], animNode->pos[f1], t) : animNode->const_pos;
            MS250::Math::Vector3 scale = (animNode->scale != nullptr) ? MS250_MATH_LERP_VECTOR3(animNode->scale[f0], animNode->scale[f1], t) : animNode->const_scale;

            //node->node_to_parent = MS250::Math::Matrix::TRS(pos, rot, scale);

            /*node->node_to_parent =
                glm::translate(
                    glm::scale(
                        glm::mat4x4(1.0f), scale
                    ) * glm::mat4_cast(rot),
                    pos
                );*/

            node->node_to_parent = MS250_MATH_TRS_MATRIX(pos, rot, scale);
        }

        for (size_t i = 0; i < this->numBlendChannelWeights; i++){
            AnimBlendChannel* animBlendChannel = &(anim->blendChannels[i]);

            this->blendChannelWeights[i] = animBlendChannel->weight ? MS250_FBX::lerp(animBlendChannel->weight[f0], animBlendChannel->weight[f1], t) : animBlendChannel->const_weight;
        }
    }
}

void MS250_FBX::UpdateHierarchy(){
    for (size_t i = 0; i<this->_numNodes; i++){
        Node * node = &(this->_nodes[i]);

        if (node->parent_index >= 0)
            node->node_to_world = this->_nodes[node->parent_index].node_to_world * node->node_to_parent;
            //node->node_to_world = glm::matrixCompMult(this->_nodes[node->parent_index].node_to_world, node->node_to_parent);
        else
            node->node_to_world = node->node_to_parent;

        node->geometry_to_world = node->node_to_world * node->geometry_to_node;
        //node->geometry_to_world = glm::matrixCompMult(node->node_to_world, node->geometry_to_node);
        //node->normal_to_world = node->geometry_to_world.Inverse().Transpose();
        //node->normal_to_world = glm::transpose(glm::inverse(node->geometry_to_world));
        node->normal_to_world = MS250_MATH_TRANSPOSE_MATRIX(MS250_MATH_INVERSE_MATRIX(node->geometry_to_world));
    }
}

void MS250_FBX::UpdateBones(){
    for (size_t mi = 0; mi < this->_numMeshes; mi++){
        for (size_t bi=0;bi<this->_meshes[mi].numBones; bi++){
            Node* bone = &(this->_nodes[this->_meshes[mi].boneIndices[bi]]); //get the bone
            MS250::Math::Matrix matrixTMP = bone->node_to_world * this->_meshes[mi].boneMatrices[bi]; //move the mesh to the bone location
            float * matrixFlat = MS250_MATH_TOVAL_MATRIX(matrixTMP); //convert matrix mesh into flat array
            std::copy(matrixFlat, matrixFlat + 16, this->_meshes[mi].boneMatricesFlat + (bi * 16));
            // for (int bmi=0;bmi<16;bmi++){
            //     this->_meshes[mi].boneMatricesFlat[(bi * 16) + bmi] = matrixFlat[bmi]; //copy to flat array
            // }
        }
    }
}
//-------------------------------

//-------------------------------
//-------------------------------
void MS250_FBX::SetAnimation(size_t animID, bool updateHierarchy){
    //reset previous animation's time to 0.
    if (this->currAnimID != -1) this->UpdateAnimation(this->currAnimID, 0.0f);

    if (animID >= 0 && animID < this->numAnimations){
        //set current animation variable to 0.
        this->currAnimID = animID;
        //set current animation time variable to 0.
        this->currAnimTime = 0.0f;
    } else {
        this->currAnimID = -1;
        this->currAnimTime = 0.0f;
    }
    //update hierarchy.
    if (updateHierarchy) this->UpdateHierarchy();
}
//-------------------------------

//-------------------------------
// Loading routines
//-------------------------------
void MS250_FBX::_LoadData(){
    MS250_FBX::LogNewLine("FBX Loaded.");
    MS250_FBX::LogNewLine("Loading data.");

    //setup limits for animations, bones, and blendshapes
    this->animation_framerate = MS250_FBX::ANIMATION_FRAMERATE;
    this->max_anim_frames = MS250_FBX::MAX_ANIM_FRAMES;
    this->max_bones = MS250_FBX::MAX_BONES;
    this->max_blendshapes = MS250_FBX::MAX_BLENDSHAPES;

    //load nodes
    MS250_FBX::LogNewLine("-Loading Nodes.");
    this->_numNodes = this->_fbx_scene->nodes.count;
    this->_nodes = new Node[this->_numNodes];

    for(size_t i=0;i<this->_numNodes;i++)
        this->_LoadNodes(this->_nodes[i], this->_fbx_scene->nodes[i]);

    //load meshes
    MS250_FBX::LogNewLine("-Loading Meshes.");
    this->_numMeshes = this->_fbx_scene->meshes.count;
    this->_meshes = new Mesh[this->_numMeshes];

    for(size_t i = 0; i< this->_numMeshes; i++){
        this->_LoadMesh(this->_meshes[i], this->_fbx_scene->meshes[i]);
    }

    this->_LoadBlendShapes();

    //this->_fbx_scene->anim_stacks;
    this->numAnimations = this->_fbx_scene->anim_stacks.count;
    this->_animations = new Animation[this->numAnimations];
    for(size_t i = 0;i < this->numAnimations; i++){
        this->_LoadAnim(this->_animations[i], this->_fbx_scene->anim_stacks.data[i], this->_fbx_scene);
    }

    this->_isLoaded = true;
}

void MS250_FBX::_LoadNodes(Node& node, ufbx_node* fbxNode){
    node.parent_index = fbxNode->parent ? fbxNode->parent->typed_id : -1;

    node.node_to_world = MS250_FBX::convertMatrix4x3to4x4(fbxNode->node_to_world);
    node.node_to_parent = MS250_FBX::convertMatrix4x3to4x4(fbxNode->node_to_parent);
    node.geometry_to_node = MS250_FBX::convertMatrix4x3to4x4(fbxNode->geometry_to_node);
    node.geometry_to_world = MS250_FBX::convertMatrix4x3to4x4(fbxNode->geometry_to_world);
    node.normal_to_world = MS250_FBX::convertNormalMatrix(&fbxNode->geometry_to_world);
}

void MS250_FBX::_LoadMesh(Mesh& mesh, ufbx_mesh* fbxMesh){

    size_t max_parts = 0;
    size_t max_triangles = 0;
    //----------------------------------
    // count number of needed parts
    //----------------------------------
    for (size_t pi=0; pi < fbxMesh->material_parts.count;pi++){
        ufbx_mesh_part* part = &fbxMesh->material_parts.data[pi];
        if (part->num_triangles == 0) continue;
        max_parts+=1;

        max_triangles = std::max(max_triangles, part->num_triangles);
    }

    //----------------------------------
    //temp buffers
    //----------------------------------
    /*size_t num_tri_indicies = fbxMesh->max_face_triangles * 3;
    uint32_t *tri_indices = new uint32_t[num_tri_indicies];*/
    //MeshVertex *vertices = new MeshVertex[max_triangles * 3];
    uint32_t indices[max_triangles * 3];

    //----------------------------------
    //setup mesh instancing
    //----------------------------------
    mesh.numInstances = fbxMesh->instances.count;
    mesh.nodeInstances = new int32_t[fbxMesh->instances.count];
    for (size_t i = 0; i < fbxMesh->instances.count; i++)
        mesh.nodeInstances[i] = (int32_t)fbxMesh->instances.data[i]->typed_id;

    //----------------------------------
    //handle skin meshes
    //----------------------------------
    size_t num_blend_shapes = 0;
    //ufbx_blend_channel *blend_channels[MAX_BLEND_SHAPES];
    ufbx_blend_channel *blend_channels[this->max_blendshapes];
    size_t num_bones = 0;
    ufbx_skin_deformer *skin = nullptr;
    SkinVertex mesh_skin_vertices[fbxMesh->num_vertices];

    if (fbxMesh->skin_deformers.count > 0){
        mesh.skinned = true;

        skin = fbxMesh->skin_deformers.data[0];

        // mesh.boneIndices = new int32_t[this->max_bones];
        // mesh.boneMatrices = new MS250::Math::Matrix[this->max_bones];

        uint32_t boneIndicesTmp[this->max_bones];
        MS250::Math::Matrix boneMatricesTmp[this->max_bones];

        //calculate number of bones and copy matricies and indices
        for (size_t ci=0; ci < skin->clusters.count; ci++){
            ufbx_skin_cluster *cluster = skin->clusters.data[ci];
            if (num_bones < this->max_bones){
                boneIndicesTmp[num_bones] = cluster->bone_node->typed_id;
                boneMatricesTmp[num_bones] = convertMatrix4x3to4x4(cluster->geometry_to_bone);
                num_bones++;
            }
        }

        mesh.boneIndices = new uint32_t[num_bones];
        std::copy(boneIndicesTmp, boneIndicesTmp + num_bones, mesh.boneIndices);
        mesh.boneMatrices = new MS250::Math::Matrix[num_bones];
        std::copy(boneMatricesTmp, boneMatricesTmp + num_bones, mesh.boneMatrices);

        /*for (int bi=0;bi<num_bones;bi++){
            mesh.boneIndices[bi] = boneIndicesTmp[bi];
            mesh.boneMatrices[bi] = boneMatricesTmp[bi];
        }*/

        mesh.boneMatricesFlat = new float[16 * max_bones];

        //copy matricies to flat array
        for (int bi=0; bi < num_bones; bi++){
            float * matrixPtr = MS250_MATH_TOVAL_MATRIX(mesh.boneMatrices[bi]); //get the bone matrix as a flat array
            std::copy(matrixPtr, matrixPtr + 16, mesh.boneMatricesFlat + (16*bi)); //copy bone matrix to flat array
            //for (int matrixI = 0; matrixI < 16; matrixI ++) mesh.boneMatricesFlat[(16*bi) + matrixI] = matrixPtr[matrixI];
        }

        mesh.numBones = num_bones;

        //handle bone index and weights of skinned verticies
        for(size_t vi=0; vi < fbxMesh->num_vertices;vi++){
            size_t num_weights = 0;

            //initialize bone indexes and weights to 0 incase there is less than 4 weights
            { //wierd edge case
                mesh_skin_vertices[vi].bone_index[0] = 0;
                mesh_skin_vertices[vi].bone_index[1] = 0;
                mesh_skin_vertices[vi].bone_index[2] = 0;
                mesh_skin_vertices[vi].bone_index[3] = 0;
                mesh_skin_vertices[vi].bone_weight[0] = 0.0f;
                mesh_skin_vertices[vi].bone_weight[1] = 0.0f;
                mesh_skin_vertices[vi].bone_weight[2] = 0.0f;
                mesh_skin_vertices[vi].bone_weight[3] = 0.0f;
            }

            ufbx_skin_vertex vertex_weights = skin->vertices.data[vi];
            for (size_t wi = 0; wi < vertex_weights.num_weights; wi++){
                if (num_weights >= 4) break;
                ufbx_skin_weight weight = skin->weights.data[vertex_weights.weight_begin + wi];

                if (weight.cluster_index < this->max_bones){
                    mesh_skin_vertices[vi].bone_index[num_weights] = (unsigned int)weight.cluster_index;
                    mesh_skin_vertices[vi].bone_weight[num_weights] = (float)weight.weight;

                    num_weights++;
                }
            }

            /*ufbx_skin_vertex vertex_weights = skin->vertices.data[vi];
            for (size_t wi=0; wi < vertex_weights.num_weights; wi++){
                if (num_weights >= 4) break;
                ufbx_skin_weight weight = skin->weights.data[vertex_weights.weight_begin + wi];

                if (weight.cluster_index < this->max_bones){
                    total_weight += (float) weight.weight;
                    clusters[num_weights] = (uint8_t)weight.cluster_index;
                    weights[num_weights] = (float)weight.weight;
                    num_weights++;
                }
            }

            if (total_weight > 0.0f){
                SkinVertex *skin_vert = &mesh_skin_vertices[vi];
                uint32_t quantized_sum = 0;
                for(size_t i=0; i < 4; i++){
                    uint8_t quantized_weight = (uint8_t)((float)weights[i] / total_weight * 255.0f);
                    quantized_sum += quantized_weight;
                    skin_vert->bone_index[i] = clusters[i];
                    skin_vert->bone_weight[i] = quantized_weight;
                }
                skin_vert->bone_weight[0] += 255 - quantized_sum;
            }*/
        }
    }

    //----------------------------------
    // handle blend shapes
    //----------------------------------
    for(size_t di = 0; di < fbxMesh->blend_deformers.count; di++){
        ufbx_blend_deformer* deformer = fbxMesh->blend_deformers.data[di];
        for(size_t ci = 0; ci < deformer->channels.count; ci++){
            ufbx_blend_channel* chan = deformer->channels.data[ci];
            if (chan->keyframes.count == 0) continue;
            //if (num_blend_shapes < MAX_BLEND_SHAPES){
            if (num_blend_shapes < this->max_blendshapes){
                blend_channels[num_blend_shapes] = chan;
                mesh.blend_channel_indices = new int32_t[this->max_blendshapes];
                mesh.blend_channel_indices[num_blend_shapes] = (int32_t)chan->typed_id;
                num_blend_shapes++;
            }
        }
    }

    if (num_blend_shapes > 0){
        mesh.num_blend_shapes = num_blend_shapes;
        mesh.blendChannels = new BlendShapeChannel[num_blend_shapes];
        
        for (size_t bci = 0; bci < num_blend_shapes; bci++){
            size_t num_keyframes = blend_channels[bci]->keyframes.count;
            mesh.blendChannels[bci].numKeyframes = num_keyframes;
            mesh.blendChannels[bci].keyframes = new BlendShapeKeyframe[num_keyframes];

            for (size_t kfi = 0; kfi < num_keyframes; kfi++){
                ufbx_blend_keyframe * keyframe = &(blend_channels[bci]->keyframes.data[kfi]);
                BlendShapeKeyframe * meshKeyframe = &(mesh.blendChannels[bci].keyframes[kfi]);
                size_t num_offsets = keyframe->shape->num_offsets;

                meshKeyframe->shape.numOffsets = num_offsets;

                meshKeyframe->targetWeight = keyframe->target_weight;
                meshKeyframe->effectiveWeight = keyframe->effective_weight;

                meshKeyframe->shape.offsetVertIndices = new uint32_t[num_offsets];
                meshKeyframe->shape.offsetPosition = new MS250::Math::Vector3[num_offsets];
                meshKeyframe->shape.offsetNormal = new MS250::Math::Vector3[num_offsets];

                for (size_t osi=0; osi < num_offsets; osi++){
                    meshKeyframe->shape.offsetVertIndices[osi] = keyframe->shape->offset_vertices[osi];

                    meshKeyframe->shape.offsetPosition[osi].x = keyframe->shape->position_offsets[osi].x;
                    meshKeyframe->shape.offsetPosition[osi].y = keyframe->shape->position_offsets[osi].y;
                    meshKeyframe->shape.offsetPosition[osi].z = keyframe->shape->position_offsets[osi].z;

                    meshKeyframe->shape.offsetNormal[osi].x = keyframe->shape->normal_offsets[osi].x;
                    meshKeyframe->shape.offsetNormal[osi].y = keyframe->shape->normal_offsets[osi].y;
                    meshKeyframe->shape.offsetNormal[osi].z = keyframe->shape->normal_offsets[osi].z;
                }
            }
        }
    }

    //----------------------------------
    // handle mesh data
    //----------------------------------
    mesh.parts = new Part[max_parts];
    size_t num_parts = 0;

    for(size_t pi = 0; pi < fbxMesh->material_parts.count; pi++){
        ufbx_mesh_part* mesh_part = &fbxMesh->material_parts.data[pi];
        if (mesh_part->num_triangles == 0) continue;

        //setup buffers
        size_t numTriangles = mesh_part->num_triangles;
        size_t num_tri_indicies = fbxMesh->max_face_triangles * 3;
        MeshVertex vertices[numTriangles * 3];
        SkinVertex skin_vertices[numTriangles * 3];
        uint32_t tri_indices[num_tri_indicies];

        Part* part = &(mesh.parts[num_parts++]);
        size_t num_indices = 0;

        for(size_t fi = 0; fi < mesh_part->num_faces; fi++){
            ufbx_face face = fbxMesh->faces.data[mesh_part->face_indices.data[fi]];

            size_t num_tris = ufbx_triangulate_face(tri_indices, num_tri_indicies, fbxMesh, face);

            ufbx_vec2 default_uv = {0};

            for (size_t vi = 0; vi < num_tris * 3; vi++){
                uint32_t ix = tri_indices[vi];
                MeshVertex *vert = &vertices[num_indices];

                ufbx_vec3 pos = ufbx_get_vertex_vec3(&fbxMesh->vertex_position, ix);
				ufbx_vec3 normal = ufbx_get_vertex_vec3(&fbxMesh->vertex_normal, ix);
                ufbx_vec2 uv = fbxMesh->vertex_uv.exists ? ufbx_get_vertex_vec2(&fbxMesh->vertex_uv, ix) : default_uv;

                //vert->position = MS250::Math::Vector3{pos.x, pos.y, pos.z};
                vert->position.x = pos.x;
                vert->position.y = pos.y;
                vert->position.z = pos.z;
                vert->normal = MS250_MATH_NORMALIZE_VECTOR3(normal.x, normal.y, normal.z);
                //vert->uv = MS250::Math::Vector2{uv.x, uv.y};
                vert->uv.x = uv.x;
                vert->uv.y = uv.y;
				vert->f_vertex_index = (float)fbxMesh->vertex_indices.data[ix];

                if (mesh.skinned){
                    skin_vertices[num_indices] = mesh_skin_vertices[fbxMesh->vertex_indices.data[ix]];
                }
                num_indices++;
            }
        }

        ufbx_vertex_stream streams[2];
        size_t num_streams = 1;
        
        streams[0].data = vertices;
        streams[0].vertex_count = num_indices;
        streams[0].vertex_size = sizeof(MeshVertex);

        if (skin){
            streams[1].data = skin_vertices;
            streams[1].vertex_count = num_indices;
            streams[1].vertex_size = sizeof(SkinVertex);
            num_streams = 2;
        }

        ufbx_error error;
        size_t num_vertices = ufbx_generate_indices(streams, num_streams, indices, num_indices, nullptr, &error);
        if (error.type != UFBX_ERROR_NONE){
            //error out
            LogNewLine("Error generating indices: ", std::string(error.description.data, 0, error.description.length));
            continue;
        }

        //setup material indexs
        part->numIndicies = num_indices;
        if (mesh_part->index < fbxMesh->materials.count){
            ufbx_material* material = fbxMesh->materials.data[mesh_part->index];
            part->materialIndex = (int32_t)material->typed_id;
        } else {
            part->materialIndex = -1;
        }

        //copy indicies
        part->indicies = new uint32_t[num_indices];
        //memcpy(mesh.parts[num_parts].indicies, indices, sizeof(uint32_t) * num_indices);
        std::copy(indices, indices + num_indices, part->indicies);
        //std::reverse_copy(indices, indices + num_indices, mesh.parts[num_parts].indicies);

        //copy vertexes
        part->vertices = new MeshVertex[num_vertices];
        part->numVertices = num_vertices;
        //memcpy(mesh.parts[num_parts].vertices, vertices, sizeof(_MeshVertex) * num_vertices);
        std::copy(vertices, vertices + num_vertices, part->vertices);

        //if available, copy skin vertexes
        if (mesh.skinned){
            part->skinVertices = new SkinVertex[num_vertices];
            //memcpy(mesh.parts[num_parts].skinVertices, skin_vertices, sizeof(_SkinVertex) * num_vertices);
            std::copy(skin_vertices, skin_vertices + num_vertices, part->skinVertices);
        }

        //copy to flat array
        //-------------------------------------------------------
        part->flatArray.positions = new float[num_vertices*3];
        part->flatArray.normals = new float[num_vertices*3];
        part->flatArray.uvs = new float[num_vertices*2];
        part->flatArray.f_vertex_index = new float[num_vertices];

        if (mesh.skinned){
            //part->flatArray.bone_indexes = new uint8_t[num_vertices*4];
            //part->flatArray.bone_weights = new uint8_t[num_vertices*4];
            //part->flatArray.bone_indexes = new unsigned int[num_vertices*4];
            //part->flatArray.bone_weights = new unsigned int[num_vertices*4];
            part->flatArray.bone_indexes = new unsigned int[num_vertices*4];
            part->flatArray.bone_weights = new float[num_vertices*4];
        }

        FlatArray * flatArray = &(part->flatArray);
        for (int i = 0; i < num_vertices; i++){
            /*float * posVal = MS250_MATH_TOVAL_VECTOR3(vertices[i].position);
            std::copy(posVal, posVal+3, flatArray->positions + (i*3));

            float * normVal = MS250_MATH_TOVAL_VECTOR3(vertices[i].normal);
            std::copy(normVal, normVal+3, flatArray->normals + (i*3));

            float * uvVal = MS250_MATH_TOVAL_VECTOR3(vertices[i].uv);
            std::copy(uvVal, uvVal+3, flatArray->uvs + (i*3));*/

            flatArray->positions[(i * 3)] = vertices[i].position.x;
            flatArray->positions[(i * 3) + 1] = vertices[i].position.y;
            flatArray->positions[(i * 3) + 2] = vertices[i].position.z;

            flatArray->normals[(i * 3)] = vertices[i].normal.x;
            flatArray->normals[(i * 3) + 1] = vertices[i].normal.y;
            flatArray->normals[(i * 3) + 2] = vertices[i].normal.z;
            
            flatArray->uvs[(i * 2)] = vertices[i].uv.x;
            flatArray->uvs[(i * 2) + 1] = vertices[i].uv.x;

            flatArray->f_vertex_index[i] = vertices[i].f_vertex_index;

            if (mesh.skinned){
                /*std::copy(skin_vertices[i].bone_index, skin_vertices[i].bone_index + 4, flatArray->bone_indexes + (i*4));
                std::copy(skin_vertices[i].bone_weight, skin_vertices[i].bone_weight + 4, flatArray->bone_weights + (i*4));*/

                flatArray->bone_indexes[(i * 4)] = skin_vertices[i].bone_index[0];
                flatArray->bone_indexes[(i * 4) + 1] = skin_vertices[i].bone_index[3];
                flatArray->bone_indexes[(i * 4) + 2] = skin_vertices[i].bone_index[1];
                flatArray->bone_indexes[(i * 4) + 3] = skin_vertices[i].bone_index[2];

                flatArray->bone_weights[(i * 4)] = skin_vertices[i].bone_weight[0];
                flatArray->bone_weights[(i * 4) + 1] = skin_vertices[i].bone_weight[3];
                flatArray->bone_weights[(i * 4) + 2] = skin_vertices[i].bone_weight[1];
                flatArray->bone_weights[(i * 4) + 3] = skin_vertices[i].bone_weight[2];
            }
        }
    }
    mesh.numParts = num_parts;

    //----------------------------------
    //delete temp buffers
    //----------------------------------
    //delete [] tri_indices;
    //delete [] vertices;
    // delete [] skin_vertices;
    // delete [] mesh_skin_vertices;
    // delete [] indices;
}

void MS250_FBX::_LoadBlendShapes(){
    this->numBlendChannelWeights = this->_fbx_scene->blend_channels.count;
    this->blendChannelWeights = new float[this->numBlendChannelWeights];

    for (size_t i = 0; i < this->_fbx_scene->blend_channels.count; i++)
        this->blendChannelWeights[i] = this->_fbx_scene->blend_channels.data[i]->weight;
}

void MS250_FBX::_LoadAnim(Animation& anim, ufbx_anim_stack * animStack, ufbx_scene * scene){
    const float target_framerate = this->animation_framerate;
    const size_t max_frames = this->max_anim_frames;

    float duration = (float)animStack->time_end - (float)animStack->time_begin;
    size_t num_frames = std::clamp((size_t)(duration * target_framerate), (size_t)2, max_frames);
    float framerate = (float)(num_frames - 1) / duration;

    anim.name = std::string(animStack->name.data, animStack->name.length);
    anim.time.begin = animStack->time_begin;
    anim.time.end = animStack->time_end;
    anim.framerate = framerate;
    anim.numFrames = num_frames;

    anim.nodes = new AnimNode[scene->nodes.count];
    for (size_t i = 0; i < scene->nodes.count; i++){
        ufbx_node *node = scene->nodes.data[i];
        this->_LoadAnimNode(anim, anim.nodes[i], animStack, node);
    }

    anim.blendChannels = new AnimBlendChannel[scene->blend_channels.count];
    for (size_t i = 0; i < scene->blend_channels.count; i++){
        ufbx_blend_channel *chan = scene->blend_channels.data[i];
        //read blend channels
        this->_LoadAnimBlendChannel(anim, anim.blendChannels[i], animStack, chan);
    }
}

void MS250_FBX::_LoadAnimNode(Animation& anim, AnimNode& animNode, ufbx_anim_stack * animStack, ufbx_node * node){
    animNode.rot = new MS250::Math::Quat[anim.numFrames];
    animNode.pos = new MS250::Math::Vector3[anim.numFrames];
    animNode.scale = new MS250::Math::Vector3[anim.numFrames];

    bool const_rot = true, const_pos = true, const_scale = true;

    for (size_t i = 0; i < anim.numFrames; i++){
        double time = animStack->time_begin + (double)i / anim.framerate;

        ufbx_transform transform = ufbx_evaluate_transform(animStack->anim, node, time);
        animNode.rot[i].x = transform.rotation.x;
        animNode.rot[i].y = transform.rotation.y;
        animNode.rot[i].z = transform.rotation.z;
        animNode.rot[i].w = transform.rotation.w;

        animNode.pos[i].x = transform.translation.x;
        animNode.pos[i].y = transform.translation.y;
        animNode.pos[i].z = transform.translation.z;

        animNode.scale[i].x = transform.scale.x;
        animNode.scale[i].y = transform.scale.y;
        animNode.scale[i].z = transform.scale.z;

        if (i > 0) {
            //if (animNode.rot[i].dot(animNode.rot[i-1]) < 0.0f){
            if (MS250_MATH_DOT_QUAT(animNode.rot[i], animNode.rot[i-1]) < 0.0f){
                animNode.rot[i] = -animNode.rot[i];
            }

            if (animNode.rot[i-1] != animNode.rot[i]) const_rot = false;
            if (animNode.pos[i-1] != animNode.pos[i]) const_pos = false;
            if (animNode.scale[i-1] != animNode.scale[i]) const_scale = false;
        }
    }

    if (const_rot){
        animNode.const_rot = animNode.rot[0];
        delete [] animNode.rot;
        animNode.rot = nullptr;
    }
    if (const_pos){
        animNode.const_pos = animNode.pos[0];
        delete [] animNode.pos;
        animNode.pos = nullptr;
    }
    if (const_scale){
        animNode.const_scale = animNode.scale[0];
        delete [] animNode.scale;
        animNode.scale = nullptr;
    }
}

void MS250_FBX::_LoadAnimBlendChannel(Animation& anim, AnimBlendChannel& animBlendChannel, ufbx_anim_stack * animStack, ufbx_blend_channel * blendChannel){
    animBlendChannel.weight = new float[anim.numFrames];
    
    bool const_weight = true;

    for(size_t i = 0; i < anim.numFrames; i++){
        double time = animStack->time_begin + (double)i / anim.framerate;
        animBlendChannel.weight[i] = (float)ufbx_evaluate_blend_weight(animStack->anim, blendChannel, time);
        if (i > 0)
            if (animBlendChannel.weight[i - 1] != animBlendChannel.weight[i])
                const_weight = false;
    }

    if (const_weight){
        animBlendChannel.const_weight = animBlendChannel.weight[0];
        delete [] animBlendChannel.weight;
        animBlendChannel.weight = nullptr;
    }
}
//-------------------------------

//-------------------------------
// Unloading routines
//-------------------------------
void MS250_FBX::_UnloadData(){
    if (this->_nodes != nullptr) delete [] this->_nodes;

    if (this->_meshes != nullptr){
        for(size_t i = 0; i < this->_numMeshes; i++)
            this->_UnloadMesh(this->_meshes[i]);
        delete [] this->_meshes;
    }

    if (this->blendChannelWeights != nullptr) delete [] this->blendChannelWeights;

    if (this->_animations != nullptr){
        for (size_t i=0;i < this->numAnimations; i++)
            this-> _UnloadAnim(this->_animations[i]);
        delete [] this->_animations;
    }
}

void MS250_FBX::_UnloadMesh(Mesh& mesh){
    if (mesh.blend_channel_indices != nullptr) delete [] mesh.blend_channel_indices;
    if (mesh.boneIndices != nullptr) delete [] mesh.boneIndices;
    if (mesh.boneMatrices != nullptr) delete [] mesh.boneMatrices;
    if (mesh.boneMatricesFlat != nullptr) delete [] mesh.boneMatricesFlat;

    if (mesh.nodeInstances != nullptr) delete [] mesh.nodeInstances;
    if (mesh.parts != nullptr){
        for(size_t i = 0; i<mesh.numParts; i++)
            _UnloadPart(mesh.parts[i]);
        delete [] mesh.parts;
    }
    if (mesh.blendChannels){
        for (size_t bci = 0; bci < mesh.num_blend_shapes; bci++){
            BlendShapeChannel * channel = &(mesh.blendChannels[bci]);
            if (channel->keyframes != nullptr){
                for (int kfi = 0; kfi < channel->numKeyframes; kfi++){
                    BlendShapeKeyframe * keyframe = &(channel->keyframes[kfi]);
                    delete [] keyframe->shape.offsetVertIndices;
                    delete [] keyframe->shape.offsetPosition;
                    delete [] keyframe->shape.offsetNormal;
                }
                delete [] channel->keyframes;
            }
        }
        delete [] mesh.blendChannels;
    }
}

void MS250_FBX::_UnloadPart(Part& part){
    if (part.vertices != nullptr) delete [] part.vertices;
    if (part.skinVertices != nullptr) delete [] part.skinVertices;
    if (part.indicies != nullptr) delete [] part.indicies;
    part.flatArray.release();
}

void MS250_FBX::_UnloadAnim(Animation& anim){
    if (anim.nodes != nullptr){
        for (size_t i = 0; i < anim.numFrames; i++){
            if (anim.nodes[i].rot != nullptr) delete [] anim.nodes[i].rot;
            if (anim.nodes[i].pos != nullptr) delete [] anim.nodes[i].pos;
            if (anim.nodes[i].scale != nullptr) delete [] anim.nodes[i].scale;
        }
        delete [] anim.nodes;
    }

    if (anim.blendChannels != nullptr){
        for (size_t i = 0; i < anim.numFrames; i++)
            if (anim.blendChannels[i].weight != nullptr) delete [] anim.blendChannels[i].weight;
        delete [] anim.blendChannels;
    }
}
//-------------------------------

//-------------------------------
// Logging funcs
//-------------------------------
template<typename... Args>
void MS250_FBX::Log(Args&&... args){
    if (MS250_FBX::debug){
        std::ostringstream oss;
        (oss << ... << args);
        std::cout<<"MS250_FBX ["<<this<<"] "<<oss.str();
    }
}

template<typename... Args>
void MS250_FBX::LogNewLine(Args&&... args){
    if (MS250_FBX::debug){
        std::ostringstream oss;
        (oss << ... << args);
        std::cout<<"MS250_FBX ["<<this<<"] "<<oss.str()<<std::endl;
    }
}
//-------------------------------

//-------------------------------
// Misc Funcs
//-------------------------------
std::string MS250_FBX::toString(std::string padding){
    std::string ret;

    ret += "Number of Nodes: " + std::to_string(this->_numNodes) + "\n-";
    for(int i = 0; i < this->_numNodes; i++){
        ret += std::to_string(i) + ":\n";
        ret += this->_nodes[i].toString(padding);
    }

    ret += "Number of Meshes: " + std::to_string(this->_numMeshes) + "\n";
    for(int i = 0; i < this->_numMeshes; i++){
        ret += std::to_string(i) + ":\n";
        ret += this->_meshes[i].toString(padding);
    }

    ret += "Number of Blend Shape Weights: " + std::to_string(this->numBlendChannelWeights) + "\n";
    ret += padding + " ";
    for (int i = 0; i < this->numBlendChannelWeights; i++){
        ret += std::to_string(this->blendChannelWeights[i]);
        if (i != this->numBlendChannelWeights && i != this->numBlendChannelWeights-1){
            ret += ", ";
        }
    }
    ret += "\n";

    ret += "Number of Animations: " + std::to_string(this->numAnimations) + "\n";
    for (int i = 0; i < this->numAnimations; i ++){
        ret += std::to_string(i) + ":\n";
        ret += this->_animations[i].toString(padding);
    }

    return ret;
}
//-------------------------------

//-------------------------------
// Static utilities
//-------------------------------
MS250::Math::Matrix MS250_FBX::convertMatrix4x3to4x4(ufbx_matrix m){
    return {
        m.m00, m.m10, m.m20, 0.0f,
        m.m01, m.m11, m.m21, 0.0f,
        m.m02, m.m12, m.m22, 0.0f,
        m.m03, m.m13, m.m23, 1.0f,
    };

    /*return {
        m.cols[0].x, m.cols[1].x, m.cols[2].x, m.cols[3].x,
        m.cols[0].y, m.cols[1].y, m.cols[2].y, m.cols[3].y,
        m.cols[0].z, m.cols[1].z, m.cols[2].z, m.cols[3].z,
        0.0f, 0.0f, 0.0f, 1.0f
    };*/

    /*return {
		(float)m.m00, (float)m.m01, (float)m.m02, (float)m.m03,
		(float)m.m10, (float)m.m11, (float)m.m12, (float)m.m13,
		(float)m.m20, (float)m.m21, (float)m.m22, (float)m.m23,
		0, 0, 0, 1
    };*/
}

MS250::Math::Matrix MS250_FBX::convertNormalMatrix(ufbx_matrix *m) {
	ufbx_real det = ufbx_matrix_determinant(m);
	ufbx_real det_sign = det >= 0.0f ? 1.0f : -1.0f;

	ufbx_matrix r;
	r.m00 = ( - m->m12*m->m21 + m->m11*m->m22) * det_sign;
	r.m01 = ( + m->m12*m->m20 - m->m10*m->m22) * det_sign;
	r.m02 = ( - m->m11*m->m20 + m->m10*m->m21) * det_sign;
	r.m10 = ( + m->m02*m->m21 - m->m01*m->m22) * det_sign;
	r.m11 = ( - m->m02*m->m20 + m->m00*m->m22) * det_sign;
	r.m12 = ( + m->m01*m->m20 - m->m00*m->m21) * det_sign;
	r.m20 = ( - m->m02*m->m11 + m->m01*m->m12) * det_sign;
	r.m21 = ( + m->m02*m->m10 - m->m00*m->m12) * det_sign;
	r.m22 = ( - m->m01*m->m10 + m->m00*m->m11) * det_sign;
	r.m03 = r.m13 = r.m23 = 0.0f;

	return convertMatrix4x3to4x4(r);
}
//-------------------------------