#include "GL_FBX.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include "MS250_FBX.h"
#include "MS250_Math.hpp"
#include "glad/gl.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/random.hpp"

//File path load
GL_FBX::GL_FBX(const char *filename, const ufbx_load_opts *opts, ufbx_error *error) : MS250_FBX(filename, opts, error){
    this->InitShader();
    // this->InitBoundingBox();
    this->Init();
}

/*Ray_FBX::Ray_FBX(const char *filename, size_t filename_len, const ufbx_load_opts *opts, ufbx_error *error) : MS250_FBX(filename, filename_len, opts, error){
    this->Init();
}

//Memory load
Ray_FBX::Ray_FBX(const void *data, size_t size, const ufbx_load_opts *opts, ufbx_error *error) : MS250_FBX(data, size, opts, error){
    this->Init();
}

//stdio load
Ray_FBX::Ray_FBX(void *file_void, const ufbx_load_opts *opts, ufbx_error *error) : MS250_FBX(file_void, opts, error){
    this->Init();
}

Ray_FBX::Ray_FBX(void *file_void, const void *prefix, size_t prefix_size, const ufbx_load_opts *opts, ufbx_error *error) : MS250_FBX(file_void, prefix, prefix_size, opts, error){
    this->Init();
}

//stream load
Ray_FBX::Ray_FBX(const ufbx_stream *stream, const ufbx_load_opts *opts, ufbx_error *error) : MS250_FBX(stream, opts, error){
    this->Init();
}

Ray_FBX::Ray_FBX(const ufbx_stream *stream, const void *prefix, size_t prefix_size, const ufbx_load_opts *opts, ufbx_error *error) : MS250_FBX(stream, prefix, prefix_size, opts, error){
    this->Init();
}*/

GL_FBX::~GL_FBX(){
    LogNewLine("Unloading Data.");
    if (this->mesh != nullptr){
        for(int mi = 0; mi < this->_numMeshes; mi++){
            /*if (this->mesh[mi].boneMatrixMap != nullptr){
            //    for (int bi = 0; bi < this->_meshes[mi].numBones; bi++){
            //         delete [] this->mesh[mi].boneMatrixMap[bi];
            //     }
                //glUnmapBuffer(this->mesh[mi].boneBuffer);
                delete [] this->mesh[mi].boneMatrixMap;
            }*/
            glDeleteBuffers(1, &this->mesh[mi].boneBuffer);

            glDeleteVertexArrays(this->_meshes[mi].numParts, this->mesh[mi].vao);
            for(int pi = 0; pi < this->_meshes[mi].numParts; pi++)
                if (this->_meshes[mi].skinned)
                    glDeleteBuffers(6, this->mesh[mi].ubuff[pi].ptr);
                else
                    glDeleteBuffers(4, this->mesh[mi].ubuff[pi].ptr);
            delete [] this->mesh[mi].ubuff;
        }
    }

    glDeleteProgram(this->shader.program);
    glDeleteShader(this->shader.vertex);
    glDeleteShader(this->shader.fragment);
}

void GL_FBX::Init(){
    if (!this->IsLoaded()){
        LogNewLine("Error! Trying to load vertex data without loading fbx.");
        return;
    }

    LogNewLine("Loading Data.");

    this->mesh = new GLMesh[this->_numMeshes];

    for(int mi = 0; mi < this->_numMeshes; mi++){
        this->mesh[mi].ubuff = new GLMesh::UBuff[this->_meshes[mi].numParts];
        this->mesh[mi].vao = new GLuint[this->_meshes[mi].numParts];
        //this->mesh[mi].colors = new float*[this->_meshes[mi].numParts];

        this->mesh[mi].color[0] = glm::linearRand(0.0f, 1.0f);
        this->mesh[mi].color[1] = glm::linearRand(0.0f, 1.0f);
        this->mesh[mi].color[2] = glm::linearRand(0.0f, 1.0f);

        if (this->_meshes[mi].skinned){
            //create bone matrix buffer
            glCreateBuffers(1, &(this->mesh[mi].boneBuffer));
            //this->mesh[mi].boneMatrixMap = new float*[this->_meshes[mi].numBones];

            //load (temporarially) data into buffers
            // glNamedBufferStorage(
            //     this->mesh[mi].boneBuffer,
            //     sizeof(float) * 16 * this->_meshes[mi].numBones,
            //     this->_meshes[mi].boneMatricesFlat,
            //     GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
            // );
            glNamedBufferStorage(
                this->mesh[mi].boneBuffer,
                sizeof(float) * 16 * this->_meshes[mi].numBones,
                nullptr,
                GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
            );

            glBindBufferRange(
                GL_SHADER_STORAGE_BUFFER,
                0,
                this->mesh[mi].boneBuffer,
                0,
                sizeof(float) * 16 * this->_meshes[mi].numBones
            );

            //glMapBuffer(this->mesh[mi].boneBuffer, GL_READ_WRITE);

            // //map the bone matricies.
            // for(int bi = 0; bi < this->_meshes[mi].numBones; bi++){
            //     this->mesh[mi].boneMatrixMap[bi] =
            //         (float*)glMapBufferRange(
            //             this->mesh[mi].boneBuffer,
            //             bi * 16,
            //             sizeof(float) * 16,
            //             GL_WRITE_ONLY
            //         );
            // }

            /*glGenBuffers(1, &(this->mesh[mi].boneBuffer));
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->mesh[mi].boneBuffer);
            glBufferData(
                GL_SHADER_STORAGE_BUFFER,
                sizeof(float) * 16 * this->_meshes[mi].numBones,
                this->_meshes[mi].boneMatricesFlat,
                GL_STATIC_DRAW
            );*/
        }

        glCreateVertexArrays(this->_meshes[mi].numParts, this->mesh[mi].vao);

        for (int pi = 0; pi < this->_meshes[mi].numParts; pi++){
            /*this->mesh[mi].colors[pi] = new float[this->_meshes[mi].parts[pi].numVertices * 3];

            //generate random colors per triangle
            for(int i=0;i<this->_meshes[mi].parts[pi].numVertices;i+=2){
                glm::vec3 col{glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f)};
                this->mesh[mi].colors[pi][(i*9)] = col.x;
                this->mesh[mi].colors[pi][(i*9)+1] = col.y;
                this->mesh[mi].colors[pi][(i*9)+2] = col.z;
            }*/

            //generate buffers
            if (this->_meshes[mi].skinned)
                glCreateBuffers(6, this->mesh[mi].ubuff[pi].ptr);
            else
                glCreateBuffers(4, this->mesh[mi].ubuff[pi].ptr);

            //load data into buffers
            glNamedBufferStorage( //vertex
                this->mesh[mi].ubuff[pi].buffers.vertex,
                sizeof(float) * 3 * this->_meshes[mi].parts[pi].numVertices,
                this->_meshes[mi].parts[pi].flatArray.positions,
                GL_DYNAMIC_STORAGE_BIT
            );
            glNamedBufferStorage( //normal
                this->mesh[mi].ubuff[pi].buffers.normal,
                sizeof(float) * 3 * this->_meshes[mi].parts[pi].numVertices,
                this->_meshes[mi].parts[pi].flatArray.normals,
                GL_DYNAMIC_STORAGE_BIT
            );
            glNamedBufferStorage( //uvs
                this->mesh[mi].ubuff[pi].buffers.uvs,
                sizeof(float) * 2 * this->_meshes[mi].parts[pi].numVertices,
                this->_meshes[mi].parts[pi].flatArray.uvs,
                GL_DYNAMIC_STORAGE_BIT
            );

            // BONE INDEXES AND WEIGHTS
            if (this->_meshes[mi].skinned){
                glNamedBufferStorage( //bone indexes
                    this->mesh[mi].ubuff[pi].buffers.bone.index,
                    sizeof(float) * 4 * this->_meshes[mi].parts[pi].numVertices,
                    this->_meshes[mi].parts[pi].flatArray.bone_indexes,
                    GL_DYNAMIC_STORAGE_BIT
                );
                glNamedBufferStorage( //bone weights
                    this->mesh[mi].ubuff[pi].buffers.bone.weight,
                    sizeof(float) * 4 * this->_meshes[mi].parts[pi].numVertices,
                    this->_meshes[mi].parts[pi].flatArray.bone_weights,
                    GL_DYNAMIC_STORAGE_BIT
                );
            }

            // glNamedBufferStorage( //colors
            //     this->mesh[mi].ubuff[pi].buffers.uvs,
            //     sizeof(float) * 3 * this->_meshes[mi].parts[pi].numVertices,
            //     this->_meshes[mi].parts[pi].flatArray.uvs,
            //     GL_DYNAMIC_STORAGE_BIT
            // );

            glNamedBufferStorage( //index
                this->mesh[mi].ubuff[pi].buffers.index,
                sizeof(uint32_t) * this->_meshes[mi].parts[pi].numIndicies,
                this->_meshes[mi].parts[pi].indicies,
                GL_DYNAMIC_STORAGE_BIT
            );

            //bind data buffers to vertex buffer
            glVertexArrayVertexBuffer( //vertex
                this->mesh[mi].vao[pi],
                0, //this->shaderUniforms.vertexPos,
                this->mesh[mi].ubuff[pi].buffers.vertex,
                0,
                sizeof(float) * 3
            );
            glVertexArrayVertexBuffer( //normal
                this->mesh[mi].vao[pi],
                1, //this->shaderUniforms.vertexNormal,
                this->mesh[mi].ubuff[pi].buffers.normal,
                0,
                sizeof(float) * 3
            );
            glVertexArrayVertexBuffer( //uvs
                this->mesh[mi].vao[pi],
                2, //this->shaderUniforms.vertexUV,
                this->mesh[mi].ubuff[pi].buffers.uvs,
                0,
                sizeof(float) * 2
            );

            // BONE INDEXES AND WEIGHTS
            if (this->_meshes[mi].skinned){
                glVertexArrayVertexBuffer( //bone indicies
                    this->mesh[mi].vao[pi],
                    3, //this->shaderUniforms.vertexUV,
                    this->mesh[mi].ubuff[pi].buffers.bone.index,
                    0,
                    sizeof(int) * 4
                );
                glVertexArrayVertexBuffer( //bone weights
                    this->mesh[mi].vao[pi],
                    4, //this->shaderUniforms.vertexUV,
                    this->mesh[mi].ubuff[pi].buffers.bone.weight,
                    0,
                    sizeof(float) * 4
                );

                //bone matricies
                /*glVertexArrayVertexBuffer(
                    this->mesh[mi].vao[pi],
                    this->shaderUniforms.bone.matricies,
                    this->mesh[mi].boneBuffer,
                    0,
                    sizeof(float) * 16
                );*/
            }

            //bind index buffer to vao
            glVertexArrayElementBuffer(this->mesh[mi].vao[pi], this->mesh[mi].ubuff[pi].buffers.index);

            //enable attributes
            glEnableVertexArrayAttrib(this->mesh[mi].vao[pi], this->shaderUniforms.vertexPos);
            glEnableVertexArrayAttrib(this->mesh[mi].vao[pi], this->shaderUniforms.vertexNormal);
            glEnableVertexArrayAttrib(this->mesh[mi].vao[pi], this->shaderUniforms.vertexUV);

            // BONE INDEXES AND WEIGHTS
            if (this->_meshes[mi].skinned){
                glEnableVertexArrayAttrib(this->mesh[mi].vao[pi], this->shaderUniforms.bone.index);
                glEnableVertexArrayAttrib(this->mesh[mi].vao[pi], this->shaderUniforms.bone.weight);

                //bone matricies
                //glEnableVertexArrayAttrib(this->mesh[mi].vao[pi], this->shaderUniforms.bone.matricies);
            }

            //setup format
            glVertexArrayAttribFormat(
                this->mesh[mi].vao[pi],
                this->shaderUniforms.vertexPos,
                3,
                GL_FLOAT,
                GL_FALSE,
                0
            );
            glVertexArrayAttribFormat(
                this->mesh[mi].vao[pi],
                this->shaderUniforms.vertexNormal,
                3,
                GL_FLOAT,
                GL_FALSE,
                0
            );
            glVertexArrayAttribFormat(
                this->mesh[mi].vao[pi],
                this->shaderUniforms.vertexUV,
                2,
                GL_FLOAT,
                GL_FALSE,
                0
            );

            // BONE INDEXES AND WEIGHTS
            if (this->_meshes[mi].skinned){
                glVertexArrayAttribFormat(
                    this->mesh[mi].vao[pi],
                    this->shaderUniforms.bone.index,
                    4,
                    GL_UNSIGNED_INT,
                    GL_FALSE,
                    0
                );
                glVertexArrayAttribFormat(
                    this->mesh[mi].vao[pi],
                    this->shaderUniforms.bone.weight,
                    4,
                    GL_FLOAT,
                    GL_FALSE,
                    0
                );

                //bone matricies
                /*glVertexArrayAttribFormat(
                    this->mesh[mi].vao[pi],
                    this->shaderUniforms.bone.matricies,
                    1,
                    GL_FLOAT,
                    GL_FALSE,
                    0
                );*/
            }

            //bind attributes
            glVertexArrayAttribBinding(
                this->mesh[mi].vao[pi],
                this->shaderUniforms.vertexPos,
                0
            );
            glVertexArrayAttribBinding(
                this->mesh[mi].vao[pi],
                this->shaderUniforms.vertexNormal,
                1
            );
            glVertexArrayAttribBinding(
                this->mesh[mi].vao[pi],
                this->shaderUniforms.vertexUV,
                2
            );

            // BONE INDEXES AND WEIGHTS
            if (this->_meshes[mi].skinned){
                glVertexArrayAttribBinding(
                    this->mesh[mi].vao[pi],
                    this->shaderUniforms.bone.index,
                    3
                );
                glVertexArrayAttribBinding(
                    this->mesh[mi].vao[pi],
                    this->shaderUniforms.bone.weight,
                    4
                );

                /*glVertexArrayAttribBinding(
                    this->mesh[mi].vao[pi],
                    this->shaderUniforms.bone.matricies,
                    0
                );*/
            }
        }

        //size_t perElementSize = sizeof(uint32_t) + (sizeof(float) * 3) * 2;
        
        /*
        // Create one large buffer
        glGenBuffers(1, &ssboId);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboId);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, totalSize, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

        // Bind the first section (e.g., positions) to binding point 0
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ssboId, 0, sizeOfPositions);

        // Bind the second section (e.g., velocities) to binding point 1
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, ssboId, sizeOfPositions, sizeOfVelocities);
        */
        
        /*this->mesh[mi].bsBuff = new GLMesh::BlendShapeBuff[this->_meshes[mi].num_blend_shapes];
        
        for (int bsi = 0; bsi < this->_meshes[mi].num_blend_shapes; bsi++){
            this->_meshes[mi].blendChannels[bsi].numKeyframes;

            glNamedBufferStorage(
                this->mesh[mi].bsBuff[bsi].buffers.position,
                sizeof(float) * 3 * this->_meshes[mi].blendChannels[],
                this->_meshes[mi].parts[pi].flatArray.positions,
                GL_DYNAMIC_STORAGE_BIT
            );
        }*/
    }
}

void GL_FBX::InitShader(){
    LogNewLine("Loading Shader.");

    this->shader.vertex = glCreateShader(GL_VERTEX_SHADER);
    this->shader.fragment = glCreateShader(GL_FRAGMENT_SHADER);

    GL_FBX::loadAndCompileShader("src/test.vs", this->shader.vertex);
    GL_FBX::loadAndCompileShader("src/test.fs", this->shader.fragment);

    this->shader.program = glCreateProgram();
    glAttachShader(this->shader.program, this->shader.vertex);
    glAttachShader(this->shader.program, this->shader.fragment);
    glLinkProgram(this->shader.program);

    if (!LinkSuccessful(this->shader.program)){
        GLint infoBuffLen;
        glGetProgramiv(this->shader.program, GL_INFO_LOG_LENGTH, &infoBuffLen);

        GLint infoLen;
        GLchar info[infoBuffLen+1];
        glGetProgramInfoLog(this->shader.program, infoBuffLen+1, &infoLen, info);

        std::ostringstream s;
        s<<"GL_FBX ["<<this<<"] Error Linking Shader: " <<info;

        glDebugMessageInsert(
            GL_DEBUG_SOURCE_THIRD_PARTY,
            GL_DEBUG_TYPE_ERROR,
            0,
            GL_DEBUG_SEVERITY_HIGH,
            -1,
            s.str().c_str()
        );
    }

    //uniforms
    this->shaderUniforms.mvp = glGetUniformLocation(this->shader.program, "mvp");
    this->shaderUniforms.color = glGetUniformLocation(this->shader.program, "vCol");
    this->shaderUniforms.geometry_to_world = glGetUniformLocation(this->shader.program, "geom2world");
    this->shaderUniforms.normal_to_world = glGetUniformLocation(this->shader.program, "norm2world");

    this->shaderUniforms.isSkinned = glGetUniformLocation(this->shader.program, "isSkinned");


    //attributes
    this->shaderUniforms.vertexPos = glGetAttribLocation(this->shader.program, "vPos");
    this->shaderUniforms.vertexNormal = glGetAttribLocation(this->shader.program, "vNorm");
    this->shaderUniforms.vertexUV = glGetAttribLocation(this->shader.program, "vUV");

    // BONE INDEXES AND WEIGHTS
    this->shaderUniforms.bone.index = glGetAttribLocation(this->shader.program, "vBoneIndex");
    this->shaderUniforms.bone.weight = glGetAttribLocation(this->shader.program, "vBoneWeight");

    //shader storage
    this->shaderUniforms.bone.ssbo = glGetProgramResourceIndex(this->shader.program, GL_SHADER_STORAGE_BLOCK, "bones_ssbo");
    glShaderStorageBlockBinding(
        this->shader.program,
        this->shaderUniforms.bone.ssbo,
        0
    );
    /*this->shaderUniforms.bone.ubo = glGetUniformBlockIndex(this->shader.program, "bones_ubo");
    glUniformBlockBinding(
        this->shader.program,
        this->shaderUniforms.bone.ubo,
        0
    );*/


    //std::cout<<this->shaderUniforms.toString("-");
}

// void GL_FBX::InitBoundingBox(){
//     this->aabb.min = {INFINITY};
//     this->aabb.max = {-INFINITY};

//     for (int mi = 0; mi < this->_numMeshes; mi++){
//         _Mesh * mesh = &(this->_meshes[mi]);


//     }
// }

void GL_FBX::Draw(glm::mat4x4 mvp){
    if (this->mesh == nullptr) return; //dont try to draw it, we have no opengl information to draw.

    glUseProgram(this->shader.program);

    //per program uniform update
    glUniformMatrix4fv(
        this->shaderUniforms.mvp,
        1,
        GL_FALSE,
        glm::value_ptr(mvp)
    );

    for(size_t mi = 0; mi < this->_numMeshes; mi++){
        Mesh* mesh = &(this->_meshes[mi]);
        
        //per mesh uniform/buffer update

        if (mesh->skinned){
            //bind bone matrix buffer
            /*glBindBuffer(
                GL_SHADER_STORAGE_BUFFER,
                this->mesh[mi].boneBuffer
            );*/
            /*glBindBufferBase(
                GL_SHADER_STORAGE_BUFFER,
                0,
                this->mesh[mi].boneBuffer
            );*/

            //generate bone matricies and update ssbo
            /*float * matrixUBO = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
            float * matrixUBO = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
            for (size_t bi=0;bi<this->_meshes[mi].numBones; bi++){
                Node* bone = &(this->_nodes[this->_meshes[mi].boneIndices[bi]]); //get the bone
                MS250::Math::Matrix matrixTMP = bone->node_to_world * this->_meshes[mi].boneMatrices[bi]; //move the mesh to the bone location
                float * matrixFlat = MS250_MATH_TOVAL_MATRIX(matrixTMP); //convert matrix mesh into flat array
                for (int bmi=0;bmi<16;bmi++){
                    matrixUBO[(bi * 16) + bmi] = matrixFlat[bmi]; //copy to buffer.
                    this->_meshes[mi].boneMatricesFlat[(bi * 16) + bmi] = matrixFlat[bmi];
                }
            }
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);*/

            //copy flat bone matricies to ssbo
            glNamedBufferSubData(
                this->mesh[mi].boneBuffer,
                0,
                sizeof(float) * 16 * mesh->numBones,
                mesh->boneMatricesFlat
            );

            /*float * matrixUBO = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
            std::copy(
                this->_meshes[mi].boneMatricesFlat,
                this->_meshes[mi].boneMatricesFlat + (16 * this->_meshes[mi].numBones), //16 floats per matrix
                matrixUBO
            );
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);*/
            /*glNamedBufferData(
                this->mesh[mi].boneBuffer,
                16 * this->_meshes[mi].numBones,
                this->_meshes[mi].boneMatricesFlat,
                GL_DYNAMIC_COPY
            );*/
            //glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

            //update ssbo
            /*glNamedBufferStorage(
                this->mesh[mi].boneBuffer,
                sizeof(float) * 16 * this->_meshes[mi].numBones,
                matrixUBO,
                GL_DYNAMIC_STORAGE_BIT
            );*/
            // glNamedBufferSubData(
            //     this->mesh[mi].boneBuffer,
            //     0,
            //     sizeof(float) * 16 * this->_meshes[mi].numBones,
            //     matrixUBO
            // );
            
            /*glUniformMatrix4fv(
                this->shaderUniforms.bone.matricies,
                this->_meshes[mi].numBones,
                GL_FALSE,
                matrixUBO
            );*/
            
            glBindBufferBase(
                GL_SHADER_STORAGE_BUFFER,
                0,
                this->mesh[mi].boneBuffer
            );
        }

        glUniform3fv(this->shaderUniforms.color, 1, this->mesh[mi].color);
        //glUniform1i(this->shaderUniforms.isSkinned, mesh->skinned ? 1 : 0); //conditional bone usage
        glUniform1i(this->shaderUniforms.isSkinned, 0); //disable bone usage

        for (size_t ni = 0; ni < mesh->numInstances; ni++){
            Node* node = &(this->_nodes[mesh->nodeInstances[ni]]);

            //per node uniform update
            glUniformMatrix4fv(
                this->shaderUniforms.geometry_to_world,
                1,
                GL_FALSE,
                //node->geometry_to_node.m
                MS250_MATH_TOVAL_MATRIX(node->geometry_to_world)
            );
            glUniformMatrix4fv(
                this->shaderUniforms.normal_to_world,
                1,
                GL_FALSE,
                //node->normal_to_world.m
                MS250_MATH_TOVAL_MATRIX(node->normal_to_world)
            );

            for (size_t pi = 0; pi < mesh->numParts; pi++){
                //per part draw vertexes
                glBindVertexArray(this->mesh[mi].vao[pi]);

                glDrawElements(
                    GL_TRIANGLES,
                    mesh->parts[pi].numIndicies,
                    GL_UNSIGNED_INT,
                    0
                );

                glBindVertexArray(0);
            }
        }
    }
    
    glUseProgram(0);
}

template<typename... Args>
void GL_FBX::Log(Args&&... args){
    if (MS250_FBX::debug){
        std::ostringstream oss;
        (oss << ... << args);
        std::cout<<"GL_FBX ["<<this<<"] "<<oss.str();
    }
}

template<typename... Args>
void GL_FBX::LogNewLine(Args&&... args){
    if (MS250_FBX::debug){
        std::ostringstream oss;
        (oss << ... << args);
        std::cout<<"GL_FBX ["<<this<<"] "<<oss.str()<<std::endl;
    }
}

std::string GL_FBX::readFile(const std::string &filepath){
    std::ifstream file(filepath);
    if (!file.is_open()){
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool GL_FBX::CompileSuccessful(int obj) {
  int status;
  glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
  return status == GL_TRUE;
}

bool GL_FBX::LinkSuccessful(int obj) {
  int status;
  glGetProgramiv(obj, GL_LINK_STATUS, &status);
  return status == GL_TRUE;
}

void GL_FBX::loadAndCompileShader(const char* filename, GLuint shader){
    std::string codeStr = GL_FBX::readFile(filename);
    const char *c_codeStr = codeStr.c_str();
    glShaderSource(shader, 1, &c_codeStr, nullptr);
    glCompileShader(shader);

    if (!GL_FBX::CompileSuccessful(shader)){
        GLint infoBuffLen;
        glGetShaderiv(shader,GL_INFO_LOG_LENGTH, &infoBuffLen);

        GLint infoLen;
        GLchar info[infoBuffLen+1];
        glGetShaderInfoLog(shader, infoBuffLen+1, &infoLen, info);

        std::ostringstream s;
        s<<"GL_FBX ["<<this<<"] Error Compiling Shader (" << filename << "): " <<info;
        glDebugMessageInsert(
            GL_DEBUG_SOURCE_THIRD_PARTY,
            GL_DEBUG_TYPE_ERROR,
            0,
            GL_DEBUG_SEVERITY_HIGH,
            -1,
            s.str().c_str()
        );
    }
}