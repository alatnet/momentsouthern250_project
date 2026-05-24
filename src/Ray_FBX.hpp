#pragma once
#include <string>
#ifndef RAY_FBX_H
#define RAY_FBX_H

#include <raylib.h>
#include <rlgl.h>

#include "glad.h"

#include "MS250_FBX.h"

class Ray_FBX : public MS250_FBX {
    public:
        //File path load
        Ray_FBX(const char *filename, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);
        // Ray_FBX(const char *filename, size_t filename_len, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);

        // //Memory load
        // Ray_FBX(const void *data, size_t size, const ufbx_load_opts *opts= nullptr, ufbx_error *error = nullptr);

        // //stdio load
        // Ray_FBX(void *file_void, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);
        // Ray_FBX(void *file_void, const void *prefix, size_t prefix_size, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);

        // //stream load
        // Ray_FBX(const ufbx_stream *stream, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);
        // Ray_FBX(const ufbx_stream *stream, const void *prefix, size_t prefix_size, const ufbx_load_opts *opts = nullptr, ufbx_error *error = nullptr);

        ~Ray_FBX();

    private:
        void Init();
        void InitShader();

    public:
        void Draw(bool drawUsingModelMethod = false, bool drawIgnoringNodes = false);

    public:
        virtual std::string toString(std::string padding = "-"){
            std::string ret = MS250_FBX::toString(padding);

            ret += this->shaderUniforms.toString(padding);

            return ret;
        }

    private:
        void _DrawMesh();
        void _DrawModel();
        void _DrawMeshIgnoreNodes();
        void _DrawModelIgnoreNodes();

        void _DrawGLMesh();

        void _DrawRLGLMesh();

        void _DrawMeshBruteForce();

    private:
        //Mesh ** partsMesh = nullptr;
        //Model ** partsModel = nullptr;
        Material mat;

        struct RLMesh {
            Mesh * partsMesh = nullptr;
            Model * partsModel = nullptr;
            //GLuint * vao = nullptr;
            //GLuint * vbo = nullptr;

            struct Buffers{
                GLuint vertex = -1;
                GLuint normal = -1;
                GLuint uvs = -1;
                GLuint index = -1;
            } *buffers;

            struct RLBuffers {
                unsigned int vertex = -1;
                unsigned int normal = -1;
                unsigned int uvs = -1;
                unsigned int index = -1;
            } *rlBuffers;

            Color color;
        } * rlMesh;

        Shader shader;
        bool shaderLoaded = false;

        struct ShaderUniforms{
            int vertexPos;
            int vertexNormal;
            int vertexUV;
            int color;
            int mvp;
            int geometry_to_world;
            int normal_to_world;
            //int world_to_clip;

            std::string toString(std::string padding = "-"){
                std::string ret = padding + "Shader Uniforms:\n";
                ret+= padding + padding + "Vertex: " + std::to_string(this->vertexPos) + "\n";
                ret+= padding + padding + "Normal: " + std::to_string(this->vertexNormal) + "\n";
                ret+= padding + padding + "UV: " + std::to_string(this->vertexUV) + "\n";
                ret+= padding + padding + "Color: " + std::to_string(this->color) + "\n";
                ret+= padding + padding + "MVP: " + std::to_string(this->mvp) + "\n";
                ret+= padding + padding + "Geometry to World: " + std::to_string(this->geometry_to_world) + "\n";
                ret+= padding + padding + "Normal to World: " + std::to_string(this->normal_to_world) + "\n";
                return ret;
            }
        } shaderUniforms;

    private:
        static Matrix CopyMatrix(float m[16]);
        static Matrix IdentityMatrix();
};

#endif