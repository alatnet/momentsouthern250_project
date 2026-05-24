#pragma once
#include "glm/ext/matrix_float4x4.hpp"
#include <cstddef>
#include <string>
#ifndef GL_FBX_H
#define GL_FBX_H

//#include "gl.h"
#include "glad/gl.h"
#include "glm/mat4x4.hpp"

#include "MS250_FBX.h"

class GL_FBX : public MS250_FBX {
    public:
        //File path load
        GL_FBX(const char *filename, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);
        // Ray_FBX(const char *filename, size_t filename_len, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);

        // //Memory load
        // Ray_FBX(const void *data, size_t size, const ufbx_load_opts *opts= nullptr, ufbx_error *error = nullptr);

        // //stdio load
        // Ray_FBX(void *file_void, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);
        // Ray_FBX(void *file_void, const void *prefix, size_t prefix_size, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);

        // //stream load
        // Ray_FBX(const ufbx_stream *stream, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);
        // Ray_FBX(const ufbx_stream *stream, const void *prefix, size_t prefix_size, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);

        ~GL_FBX();
    private:
        void Init();
        //void InitBoundingBox();
        void InitShader();

    public:
        void Draw(glm::mat4x4 mvp);

    public:
        virtual std::string toString(std::string padding = "-"){
            std::string ret = MS250_FBX::toString(padding);

            ret += padding + "Buffers:\n";
            for(size_t mi=0; mi < this->_numMeshes; mi++){
                ret += padding + std::to_string(mi) + ":\n";
                ret += this->mesh[mi].toString(padding + padding, this->_meshes[mi].numParts);
            }
            ret += "\n";

            ret += this->shaderUniforms.toString(padding);

            return ret;
        }

    private:
        struct GLMesh {
            // struct Buffers{
            //     GLuint vao = -1;
            //     GLuint vertex = -1;
            //     GLuint normal = -1;
            //     GLuint uvs = -1;
            //     GLuint index = -1;
            // } *buffers;

            union UBuff {
                constexpr UBuff() {}
                struct Buffers{
                    GLuint vertex = -1;
                    GLuint normal = -1;
                    GLuint uvs = -1;
                    //GLuint color = -1;
                    GLuint index = -1;
                    struct Bone {
                        GLuint index = -1;
                        GLuint weight = -1;
                    } bone;
                } buffers;
                GLuint ptr[6];

                std::string toString(std::string padding = "-"){
                    std::string ret = "";

                    ret += padding + "Vertex: " + std::to_string(this->buffers.vertex) + "\n";
                    ret += padding + "Normal: " + std::to_string(this->buffers.normal) + "\n";
                    ret += padding + "UVs: " + std::to_string(this->buffers.uvs) + "\n";
                    ret += padding + "Index: " + std::to_string(this->buffers.index) + "\n";
                    ret += padding + "Bone Index: " + std::to_string(this->buffers.bone.index) + "\n";
                    ret += padding + "Weight: " + std::to_string(this->buffers.bone.weight) + "\n";

                    return ret;
                }
            } *ubuff;

            GLuint *vao;
            GLuint boneBuffer;
            //float ** boneMatrixMap = nullptr;

            float ** colors = nullptr;

            /*union BlendShapeBuff {
                struct Buffers {
                    GLuint indices;
                    GLuint position;
                    GLuint normal;
                } buffers;
                GLuint ptr[3];
            } *bsBuff;*/

            float color[4] = {0.0f, 0.0f, 1.0f, 1.0f};

            std::string toString(std::string padding = "-", size_t numParts = 0){
                std::string ret = "";

                ret += padding + "Bone Matricies Buffer: " + std::to_string(boneBuffer) + "\n";

                ret += padding + "VAOs: ";
                for (size_t i = 0; i < numParts; i++){
                    ret += std::to_string(vao[i]) + ", ";
                }
                ret += "\n";

                ret += padding + "Buffers:\n";
                for (size_t i = 0; i < numParts; i++){
                    ret += padding + std::to_string(i) + ":\n";
                    ret += this->ubuff[i].toString(padding + padding);
                }

                return ret + "\n";
            }
        } * mesh;

        struct GLShader {
            GLuint program;
            GLuint fragment;
            GLuint vertex;
        } shader;

        struct ShaderUniforms{
            int vertexPos = -1;
            int vertexNormal = -1;
            int vertexUV = -1;
            //int vertexColor = -1;
            int color = -1;
            int mvp = -1;
            int geometry_to_world = -1;
            int normal_to_world = -1;
            int isSkinned = -1;

            struct Bone{
                //int use = -1;
                int index = -1;
                int weight = -1;
                int ssbo = -1;
                //int ubo = -1;
            } bone;

            std::string toString(std::string padding = "-"){
                std::string ret = padding + "Shader Uniforms:\n";
                ret+= padding + padding + "Vertex: " + std::to_string(this->vertexPos) + "\n";
                ret+= padding + padding + "Normal: " + std::to_string(this->vertexNormal) + "\n";
                ret+= padding + padding + "UV: " + std::to_string(this->vertexUV) + "\n";
                ret+= padding + padding + "Color: " + std::to_string(this->color) + "\n";
                ret+= padding + padding + "MVP: " + std::to_string(this->mvp) + "\n";
                ret+= padding + padding + "Geometry to World: " + std::to_string(this->geometry_to_world) + "\n";
                ret+= padding + padding + "Normal to World: " + std::to_string(this->normal_to_world) + "\n";
                ret+= padding + padding + "Bone Index: " + std::to_string(this->bone.index) + "\n";
                ret+= padding + padding + "Bone Weight: " + std::to_string(this->bone.weight) + "\n";
                ret+= padding + padding + "Bone SSBO: " + std::to_string(this->bone.ssbo) + "\n";
                //ret+= padding + padding + "Bone UBO: " + std::to_string(this->bone.ubo) + "\n";
                //ret+= padding + padding + "Bone Use: " + std::to_string(this->bone.use) + "\n";
                return ret;
            }
        } shaderUniforms;

    protected:
        template<typename... Args>
        void Log(Args&&... args);

        template<typename... Args>
        void LogNewLine(Args&&... args);

    private:
        /*static Matrix CopyMatrix(float m[16]);
        static Matrix IdentityMatrix();*/
        static std::string readFile(const std::string &filepath);
        static bool CompileSuccessful(int obj);
        static bool LinkSuccessful(int obj);
        void loadAndCompileShader(const char* filename, GLuint shader);
};

#endif