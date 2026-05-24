#pragma once
#ifndef MS250_STRUCTS_H
#define MS250_STRUCTS_H

#include <string>

#include "MS250_Math.hpp"

namespace MS250{
    namespace Structs{
        struct MeshVertex{
            MS250::Math::Vector3 position; //x, y, z
            MS250::Math::Vector3 normal; //nx, ny, nz
            MS250::Math::Vector2 uv; //u, v
            float f_vertex_index = 0.0f;

            bool operator==(MeshVertex a){
                if (this->position != a.position) return false;
                if (this->normal != a.normal) return false;
                if (this->uv != a.uv) return false;
                if (this->f_vertex_index != a.f_vertex_index) return false;
                return true;
            }

            std::string toString(std::string padding = "-"){
                std::string ret = "";

                ret += padding + "Position: (" + 
                    std::to_string(this->position.x) + ", " +
                    std::to_string(this->position.y) + ", " +
                    std::to_string(this->position.z) + ")\n";

                ret += padding + "Normal: (" + 
                    std::to_string(this->normal.x) + ", " +
                    std::to_string(this->normal.y) + ", " +
                    std::to_string(this->normal.z) + ")\n";

                ret += padding + "UVs: (" + 
                    std::to_string(this->uv.x) + ", " +
                    std::to_string(this->uv.y) + ")\n";

                ret += padding + "f_vertex_index: " + std::to_string(f_vertex_index) + "\n";

                return ret;
            }
        };

        struct SkinVertex {
            unsigned int bone_index[4];
            float bone_weight[4];

            std::string toString(std::string padding = "-"){
                std::string ret = "";

                ret += padding + "Bone Index: (" +
                    std::to_string(this->bone_index[0]) + ", " +
                    std::to_string(this->bone_index[1]) + ", " +
                    std::to_string(this->bone_index[2]) + ", " +
                    std::to_string(this->bone_index[3]) + ")\n";

                ret += padding + "Bone Weight: (" +
                    std::to_string(this->bone_weight[0]) + ", " +
                    std::to_string(this->bone_weight[1]) + ", " +
                    std::to_string(this->bone_weight[2]) + ", " +
                    std::to_string(this->bone_weight[3]) + ")\n";

                return ret;
            }
        };

        struct FlatArray{
            //MeshVertex
            float * positions = nullptr; //3 positions per vertex
            float * normals = nullptr; //3 normals per vertex
            float * uvs = nullptr; //3 uvs per vertex
            float * f_vertex_index = nullptr; //1 vertex index per vertex

            //SkinVertex
            unsigned int * bone_indexes = nullptr; //4 indices per vertex
            float * bone_weights = nullptr; //4 weights per vertex

            void release(){
                if (this->positions != nullptr) delete [] this->positions;
                if (this->normals != nullptr) delete [] this->normals;
                if (this->uvs != nullptr) delete [] this->uvs;
                if (this->f_vertex_index != nullptr) delete [] this->f_vertex_index;
                if (this->bone_indexes != nullptr) delete [] this->bone_indexes;
                if (this->bone_weights != nullptr) delete [] this->bone_weights;
            }
        }; 

        struct Part {
            MeshVertex* vertices = nullptr;
            SkinVertex* skinVertices = nullptr;
            uint32_t *indicies = nullptr;

            FlatArray flatArray;

            size_t numVertices = 0;
            size_t numIndicies = 0;

            int32_t materialIndex;

            /*void loadToFlatArray(bool skinned = false){
                this->flatArray.positions = new float[numVertices * 3];
                this->flatArray.normals = new float[numVertices * 3];
                this->flatArray.uvs = new float[numVertices * 2];
                this->flatArray.f_vertex_index = new float[numVertices];

                if (skinned){
                    this->flatArray.bone_indexes = new uint8_t[numVertices * 4];
                    this->flatArray.bone_weights = new uint8_t[numVertices * 4];
                }

                for (int i=0,pi=0,ui=0,bi=0;i<numVertices;i++,pi+=3,ui+=2,bi+=4){
                    this->flatArray.positions[pi] = this->vertices[i].position[0];
                    this->flatArray.positions[pi+1] = this->vertices[i].position[1];
                    this->flatArray.positions[pi+2] = this->vertices[i].position[2];

                    this->flatArray.normals[pi] = this->vertices[i].normal[0];
                    this->flatArray.normals[pi+1] = this->vertices[i].normal[1];
                    this->flatArray.normals[pi+2] = this->vertices[i].normal[2];

                    this->flatArray.uvs[ui] = this->vertices[i].uv[0];
                    this->flatArray.uvs[ui+1] = this->vertices[i].uv[1];
                    
                    this->flatArray.f_vertex_index[i] = this->vertices[i].f_vertex_index;

                    if (skinned){
                        this->flatArray.bone_indexes[bi] = this->skinVertices[i].bone_index[0];
                        this->flatArray.bone_indexes[bi+1] = this->skinVertices[i].bone_index[1];
                        this->flatArray.bone_indexes[bi+2] = this->skinVertices[i].bone_index[2];
                        this->flatArray.bone_indexes[bi+3] = this->skinVertices[i].bone_index[3];
                        this->flatArray.bone_weights[bi] = this->skinVertices[i].bone_weight[0];
                        this->flatArray.bone_weights[bi+1] = this->skinVertices[i].bone_weight[1];
                        this->flatArray.bone_weights[bi+2] = this->skinVertices[i].bone_weight[2];
                        this->flatArray.bone_weights[bi+3] = this->skinVertices[i].bone_weight[3];
                    }
                }
            }*/

            void releaseStructs(){
                if (this->vertices != nullptr) delete [] this->vertices;
                if (this->skinVertices != nullptr) delete [] this->skinVertices;
            }

            std::string toString(std::string padding = "-"){
                std::string ret = "";

                ret += padding + "Number of Vertices: " + std::to_string(this->numVertices) + "\n";
                for (int i = 0; i < this->numVertices; i++){
                    ret += padding + std::to_string(i) + ":\n";
                    ret += this->vertices[i].toString(padding + padding);
                }

                if (this->skinVertices != nullptr){
                    ret += padding + "Skin Verticies:\n";
                    for (int i = 0; i < this->numVertices; i++){
                        ret += padding + std::to_string(i) + ":\n";
                        ret += this->skinVertices[i].toString(padding + padding);
                    }
                }

                ret += padding + "Number of Indicies: " + std::to_string(this->numIndicies) + "\n";
                if (this->numIndicies > 0){
                    ret += padding + padding;
                    for (int i = 0; i < this->numIndicies; i++){
                        ret += std::to_string(this->indicies[i]);
                        if (i != this->numIndicies && i != this->numIndicies-1){
                            ret += ", ";
                        }
                    }
                    ret += "\n";
                }

                ret += padding + "Material Index: " + std::to_string(this->materialIndex) + "\n";

                return ret;
            }
        };

        struct BlendShape {
            size_t numOffsets = 0;
            uint32_t * offsetVertIndices = nullptr;
            MS250::Math::Vector3 * offsetPosition;
            MS250::Math::Vector3 * offsetNormal;

            std::string toString(std::string padding = "-"){
                std::string ret = "";

                ret += padding + "Number of Offsets: " + std::to_string(this->numOffsets) + "\n";

                for(int i=0; i<this->numOffsets; i++){
                    ret += padding + "Shape [" + std::to_string(i) + "]: ";
                    ret += "Index: " + std::to_string(this->offsetVertIndices[i]) + " - ";
                    ret += "Position: (" + 
                        std::to_string(this->offsetPosition[i].x) + ", " +
                        std::to_string(this->offsetPosition[i].y) + ", " +
                        std::to_string(this->offsetPosition[i].z) + ")";

                    ret += " - Normal: (" + 
                        std::to_string(this->offsetNormal[i].x) + ", " +
                        std::to_string(this->offsetNormal[i].y) + ", " +
                        std::to_string(this->offsetNormal[i].z) + ")\n";
                }

                return ret;
            }
        };

        struct BlendShapeKeyframe {
            float targetWeight = 0.0f;
            float effectiveWeight = 0.0f;
            BlendShape shape;

            std::string toString(std::string padding = "-"){
                std::string ret = "";

                ret += padding + "Target Weight: " + std::to_string(this->targetWeight) + "\n";
                ret += padding + "Effective Weight: " + std::to_string(this->effectiveWeight) + "\n";
                ret += padding + "Shapes:\n";
                ret += shape.toString(padding + padding);

                return ret;
            }
        };

        struct BlendShapeChannel {
            float weight = 0.0f;

            size_t numKeyframes = 0;
            BlendShapeKeyframe * keyframes = nullptr;

            std::string toString(std::string padding = "-"){
                std::string ret = "";

                ret += padding + "Weight: " + std::to_string(this->weight) + "\n";
                ret += padding + "Num Keyframes: " + std::to_string(this->numKeyframes) + "\n";
                ret += padding + "Keyframes:\n";

                for(int i = 0; i < this->numKeyframes; i++)
                    ret += keyframes[i].toString(padding + padding) + "\n";

                return ret;
            }
        };

        struct Mesh {
            int32_t * nodeInstances = nullptr; //array of nodes that use this mesh.
            size_t numInstances = 0;

            Part* parts = nullptr;
            size_t numParts = 0;
            
            // Skinning (optional)
            bool skinned = false;
            size_t numBones = 0;

            uint32_t * boneIndices = nullptr;
            MS250::Math::Matrix * boneMatrices = nullptr;
            float * boneMatricesFlat = nullptr;

            // Blend shapes (optional)
            size_t num_blend_shapes = 0;
            BlendShapeChannel * blendChannels = nullptr;

            int32_t * blend_channel_indices = nullptr;

            std::string toString(std::string padding = "-"){
                std::string ret = "";

                ret += padding + "Number of Node Instances: " + std::to_string(this->numInstances) + "\n";
                if (this->numInstances > 0){
                    ret += padding + padding;
                    for (int i = 0; i < this->numInstances; i++){
                        ret += std::to_string(this->nodeInstances[i]);
                        if (i != this->numInstances && i != this->numInstances-1){
                            ret += ", ";
                        }
                    }
                    ret += "\n";
                }

                ret += padding + "Number of Parts: " + std::to_string(this->numParts) + "\n";
                for (int i = 0; i < this->numParts; i++){
                    ret += padding + std::to_string(i) + ":\n";
                    ret += this->parts[i].toString(padding + padding);
                }

                ret += padding + "Skinned: " + (this->skinned ? "Yes" : "No") + "\n";

                if (this->skinned){
                    ret += padding + "Number of Bones: " + std::to_string(this->numBones) + "\n";
                    ret += padding + "Bone Indicies: ";
                    for (int i=0;i<this->numBones; i++) ret += std::to_string(this->boneIndices[i]) + ", ";

                    for (int i=0;i<this->numBones;i++){
                        ret += padding + std::to_string(i) + ":\n";
                        ret += MS250::Math::matrixToString(this->boneMatrices[i], padding + padding);
                    }

                    ret += padding + "Flat Array Bones:\n";
                    for (int i=0;i<this->numBones*16;i++){
                        if (i % 4 == 0 && i != 0){ ret += "\n"; }
                        if (i % 16 == 0 && i != 0){ ret += "\n"; }

                        ret += std::to_string(this->boneMatricesFlat[i]) + ", ";
                    }
                    ret += "\n";
                }

                ret += padding + "Number of Blend Shapes: " + std::to_string(this->num_blend_shapes) + "\n";
                if (this->num_blend_shapes > 0){
                    ret += padding + padding;
                    //for (int i = 0; i < this->num_blend_shapes && i < MAX_BLEND_SHAPES; i++){
                    for (int i = 0; i < this->num_blend_shapes; i++){
                        ret += std::to_string(this->blend_channel_indices[i]);
                        if (i != this->num_blend_shapes && i != this->numInstances-1){
                            ret += ", ";
                        }
                    }
                    ret += "\n";
                }

                ret += padding + "Blend Channels: \n";
                for (int i=0; i< this->num_blend_shapes; i++)
                    ret += this->blendChannels[i].toString(padding + padding) + "\n";

                ret += padding + "Blend Shape Indices: ";
                //for (int i = 0; i < MAX_BLEND_SHAPES; i++){
                for (int i = 0; i < this->num_blend_shapes; i++){
                    ret += std::to_string(blend_channel_indices[i]);
                    if (i != this->num_blend_shapes && i != this->num_blend_shapes-1){
                        ret += ", ";
                    }
                }
                ret += "\n";

                return ret;
            }
        };

        struct Node {
            int32_t parent_index = 0;

            //matricies
            MS250::Math::Matrix geometry_to_node = {0};
            MS250::Math::Matrix node_to_parent = {0};
            MS250::Math::Matrix node_to_world = {0};
            MS250::Math::Matrix geometry_to_world = {0};
            MS250::Math::Matrix normal_to_world = {0};

            std::string toString(std::string padding = "-"){
                std::string ret = "";

                ret += padding + "Parent Index: " + std::to_string(this->parent_index) + "\n";
                // ret += padding + "Geometry to Node:\n" + this->geometry_to_node.toString(padding + padding);
                // ret += padding + "Node to Parent:\n" + this->node_to_parent.toString(padding + padding);
                // ret += padding + "Node to World:\n" + this->node_to_world.toString(padding + padding);
                // ret += padding + "Geometry to World:\n" + this->geometry_to_world.toString(padding + padding);
                // ret += padding + "Normal to World:\n" + this->normal_to_world.toString(padding + padding);

                ret += padding + "Geometry to Node:\n" + MS250::Math::matrixToString(this->geometry_to_node, padding + padding);
                ret += padding + "Node to Parent:\n" + MS250::Math::matrixToString(this->node_to_parent, padding + padding);
                ret += padding + "Node to World:\n" + MS250::Math::matrixToString(this->node_to_world, padding + padding);
                ret += padding + "Geometry to World:\n" + MS250::Math::matrixToString(this->geometry_to_world, padding + padding);
                ret += padding + "Normal to World:\n" + MS250::Math::matrixToString(this->normal_to_world, padding + padding);

                return ret;
            }
        };

        struct AnimNode {
            float time_begin = 0.0f;
            float framerate = 0.0f;
            size_t numFrames = 0;
            MS250::Math::Quat const_rot;
            MS250::Math::Vector3 const_pos;
            MS250::Math::Vector3 const_scale;
            MS250::Math::Quat * rot = nullptr;
            MS250::Math::Vector3 * pos = nullptr;
            MS250::Math::Vector3 * scale = nullptr;
        };

        struct AnimBlendChannel {
            float const_weight = 0.0f;
            size_t numWeights = 0;
            float * weight = nullptr;
        };

        struct Animation {
            std::string name;
            struct Time {
                float begin = 0.0f;
                float end = 0.0f;
            } time;
            float framerate = 0.0f;
            size_t numFrames = 0;

            AnimNode * nodes = nullptr;
            AnimBlendChannel * blendChannels = nullptr;

            std::string toString(std::string padding = "-"){
                std::string ret = "";

                return ret;
            }
        };
    }
}

#endif