#include "Ray_FBX.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <raylib.h>
#include <iostream>
#include <rlgl.h>
#include <raymath.h>

//File path load
Ray_FBX::Ray_FBX(const char *filename, const ufbx_load_opts *opts, ufbx_error *error) : MS250_FBX(filename, opts, error){
    this->Init();
    this->InitShader();
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

Ray_FBX::~Ray_FBX(){
    std::cout<<"RayFBX ["<<this<<"] Unloading Data."<<std::endl;
    if (rlMesh != nullptr){
        for(int mi = 0; mi < this->_numMeshes; mi++){
            /*delete [] rlMesh->partsMesh;
            delete [] rlMesh->partsModel;*/

            for(int pi = 0; pi < this->_meshes[mi].numParts; pi++){
                //glDeleteBuffers(1, &(rlMesh[mi].vbo[pi]));
                //glDeleteVertexArrays(1, &(rlMesh[mi].vao[pi]));
                glDeleteBuffers(1, &(rlMesh[mi].buffers[pi].vertex));
                glDeleteBuffers(1, &(rlMesh[mi].buffers[pi].normal));
                glDeleteBuffers(1, &(rlMesh[mi].buffers[pi].uvs));
                glDeleteBuffers(1, &(rlMesh[mi].buffers[pi].index));
            }

            // delete [] rlMesh[mi].vao;
            // delete [] rlMesh[mi].vbo;
            delete [] rlMesh[mi].buffers;

            if (rlMesh[mi].partsMesh != nullptr){
                for(int pi = 0; pi < this->_meshes[mi].numParts; pi++){
                    //for (int pi = 0; pi < this->_meshes[mi].numParts; pi++) UnloadMesh(partsMesh[mi][pi]);
                    if (rlMesh[mi].partsMesh[pi].vertices != nullptr) delete [] rlMesh[mi].partsMesh[pi].vertices;
                    if (rlMesh[mi].partsMesh[pi].normals != nullptr) delete [] rlMesh[mi].partsMesh[pi].normals;
                    if (rlMesh[mi].partsMesh[pi].indices != nullptr) delete [] rlMesh[mi].partsMesh[pi].indices;
                }

                delete [] rlMesh[mi].partsMesh;
            }

            if (rlMesh[mi].partsModel != nullptr){
                /*for(int mi = 0; mi < this->_numMeshes; mi++){
                    //for (int pi = 0; pi < this->_meshes[mi].numParts; pi++) UnloadModel(partsModel[mi][pi]);
                }*/

                delete [] rlMesh[mi].partsModel;
            }
        }
    }

    if (this->shaderLoaded) UnloadShader(this->shader);

    UnloadMaterial(this->mat);
}

void Ray_FBX::Init(){
    if (!this->IsLoaded()){
        std::cout<<"RayFBX ["<<this<<"] Error! Trying to load vertex data without loading fbx."<<std::endl;
        return;
    }

    std::cout<<"RayFBX ["<<this<<"] Loading Data."<<std::endl;

    this->rlMesh = new RLMesh[this->_numMeshes];

    for(int mi = 0; mi < this->_numMeshes; mi++){
        this->rlMesh[mi].partsMesh = new Mesh[this->_meshes[mi].numParts];
        this->rlMesh[mi].partsModel = new Model[this->_meshes[mi].numParts];

        // this->rlMesh[mi].vao = new GLuint[this->_meshes[mi].numParts];
        // this->rlMesh[mi].vbo = new GLuint[this->_meshes[mi].numParts];

        this->rlMesh[mi].buffers = new RLMesh::Buffers[this->_meshes[mi].numParts];

        //this->rlMesh[mi].rlBuffers = new RLMesh::RLBuffers[this->_meshes[mi].numParts];

        this->rlMesh[mi].color.r = GetRandomValue(0, 255);
        this->rlMesh[mi].color.g = GetRandomValue(0, 255);
        this->rlMesh[mi].color.b = GetRandomValue(0, 255);
        this->rlMesh[mi].color.a = 255;

        for (int pi = 0; pi < this->_meshes[mi].numParts; pi++){
            this->rlMesh[mi].partsMesh[pi].vertexCount = this->_meshes[mi].parts[pi].numVertices * 3;

            this->rlMesh[mi].partsMesh[pi].vertices = new float[this->_meshes[mi].parts[pi].numVertices * 3];
            this->rlMesh[mi].partsMesh[pi].normals = new float[this->_meshes[mi].parts[pi].numVertices * 3];
            this->rlMesh[mi].partsMesh[pi].indices = new unsigned short[this->_meshes[mi].parts[pi].numIndicies];

            //copy verticies
            //this->partsMesh[mi][pi].vertices = this->_meshes[mi].parts[pi].flatArray.positions;
            std::copy(
                this->_meshes[mi].parts[pi].flatArray.positions,
                this->_meshes[mi].parts[pi].flatArray.positions + (this->_meshes[mi].parts[pi].numVertices * 3),
                this->rlMesh[mi].partsMesh[pi].vertices
            );

            //copy normals
            //this->partsMesh[mi][pi].normals = this->_meshes[mi].parts[pi].flatArray.normals;
            std::copy(
                this->_meshes[mi].parts[pi].flatArray.normals,
                this->_meshes[mi].parts[pi].flatArray.normals + (this->_meshes[mi].parts[pi].numVertices * 3),
                this->rlMesh[mi].partsMesh[pi].normals
            );

            //copy indices
            //for (int i=0;i<this->_meshes[mi].parts[pi].numIndicies;i++) this->partsMesh[mi][pi].indices[i] = this->_meshes[mi].parts[pi].indicies[i];
            std::copy(
                this->_meshes[mi].parts[pi].indicies,
                this->_meshes[mi].parts[pi].indicies + this->_meshes[mi].parts[pi].numIndicies,
                this->rlMesh[mi].partsMesh[pi].indices
            );

            //set triangle count
            this->rlMesh[mi].partsMesh[pi].triangleCount = this->_meshes[mi].parts[pi].numVertices;

            //UploadMesh(&(this->partsMesh[mi][pi]), false);
            this->rlMesh[mi].partsModel[pi] = LoadModelFromMesh(this->rlMesh[mi].partsMesh[pi]);

            /*this->rlMesh[mi].rlBuffers[pi].vertex = rlLoadVertexBuffer(this->_meshes[mi].parts[pi].flatArray.positions, this->_meshes[mi].parts[pi].numVertices * 3, false);
            this->rlMesh[mi].rlBuffers[pi].normal = rlLoadVertexBuffer(this->_meshes[mi].parts[pi].flatArray.normals, this->_meshes[mi].parts[pi].numVertices * 3, false);
            this->rlMesh[mi].rlBuffers[pi].index = rlLoadVertexBufferElement(this->_meshes[mi].parts[pi].indicies, this->_meshes[mi].parts[pi].numIndicies, false);*/

            /*glGenVertexArrays(1, &(this->rlMesh[mi].vao[pi]));
            glBindVertexArray(this->rlMesh[mi].vao[pi]);
                glGenBuffers(1, &(this->rlMesh[mi].vbo[pi]));
                
                glBindBuffer(GL_ARRAY_BUFFER, this->rlMesh[mi].vbo[pi]);
                glBufferData(
                    GL_ARRAY_BUFFER,
                    this->_meshes[mi].parts[pi].numVertices * 3,
                    this->_meshes[mi].parts[pi].flatArray.positions,
                    GL_STATIC_DRAW
                );

                glVertexAttribPointer(shader.locs[SHADER_LOC_VERTEX_POSITION], 3, GL_FLOAT, GL_FALSE, 0, 0);
                glEnableVertexAttribArray(0);*/

                #define GenBuffers(buffer, bufferType, size, data, usage) \
                glGenBuffers(1, &(buffer)); \
                glBindBuffer(bufferType, buffer); \
                glBufferData( \
                    bufferType, \
                    size, \
                    data, \
                    usage \
                )

                //---------------------------------
                GenBuffers(
                    this->rlMesh[mi].buffers[pi].vertex,
                    GL_ARRAY_BUFFER,
                    sizeof(float) * (this->_meshes[mi].parts[pi].numVertices * 3),
                    this->_meshes[mi].parts[pi].flatArray.positions,
                    GL_STATIC_DRAW
                );

                GenBuffers(
                    this->rlMesh[mi].buffers[pi].normal,
                    GL_ARRAY_BUFFER,
                    sizeof(float) * (this->_meshes[mi].parts[pi].numVertices * 3),
                    this->_meshes[mi].parts[pi].flatArray.normals,
                    GL_STATIC_DRAW
                );

                GenBuffers(
                    this->rlMesh[mi].buffers[pi].uvs,
                    GL_ARRAY_BUFFER,
                    sizeof(float) * (this->_meshes[mi].parts[pi].numVertices * 2),
                    this->_meshes[mi].parts[pi].flatArray.uvs,
                    GL_STATIC_DRAW
                );

                GenBuffers(
                    this->rlMesh[mi].buffers[pi].index,
                    GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(uint32_t) * this->_meshes[mi].parts[pi].numIndicies,
                    this->_meshes[mi].parts[pi].indicies,
                    GL_STATIC_DRAW
                );
                /*glGenBuffers(1, &(this->rlMesh[mi].buffers[pi].vertex));
                glBindBuffer(GL_ARRAY_BUFFER, this->rlMesh[mi].buffers[pi].vertex);
                glBufferData(
                    GL_ARRAY_BUFFER,
                    sizeof(float) * (this->_meshes[mi].parts[pi].numVertices * 3),
                    this->_meshes[mi].parts[pi].flatArray.positions,
                    GL_STATIC_DRAW
                );*/

                /*glGenBuffers(1, &(this->rlMesh[mi].buffers[pi].index));
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->rlMesh[mi].buffers[pi].index);
                glBufferData(
                    GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(uint32_t) * (this->_meshes[mi].parts[pi].numIndicies),
                    this->_meshes[mi].parts[pi].indicies,
                    GL_STATIC_DRAW
                );*/
                //---------------------------------

                #undef GenBuffers


                /*glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);*/
        }
    }

    this->mat = LoadMaterialDefault();
    this->mat.maps[0].color = BLUE;
}

void Ray_FBX::InitShader(){
    std::cout<<"RayFBX ["<<this<<"] Loading Shader."<<std::endl;
    this->shader = LoadShader("src/test.vs", "src/test.fs");
    this->shaderLoaded = IsShaderValid(this->shader);

    if (this->shaderLoaded) {
        std::cout<<"RayFBX ["<<this<<"] Shader is Valid."<<std::endl;
    }

    this->shaderUniforms.vertexPos = GetShaderLocation(shader, "vertexPosition");
    this->shaderUniforms.vertexNormal = GetShaderLocation(shader, "vertexNormal");
    this->shaderUniforms.vertexUV = GetShaderLocation(shader, "vertexUV");
    this->shaderUniforms.color = GetShaderLocation(shader, "color");
    this->shaderUniforms.mvp = GetShaderLocation(shader, "mvp");
    this->shaderUniforms.geometry_to_world = GetShaderLocation(shader, "geometry_to_world");
    this->shaderUniforms.normal_to_world = GetShaderLocation(shader, "normal_to_world");
    //this->shaderUniforms.world_to_clip = GetShaderLocation(shader, "world_to_clip");
}

void Ray_FBX::Draw(bool drawUsingModelMethod, bool drawIgnoringNodes){
    _DrawMeshBruteForce();
    /*if (this->shaderLoaded) this->_DrawGLMesh();
    else
        drawIgnoringNodes ?
            (drawUsingModelMethod ? this->_DrawModelIgnoreNodes() : this->_DrawMeshIgnoreNodes()) :
            (drawUsingModelMethod ? this->_DrawModel() : this->_DrawMesh());*/
}

void Ray_FBX::_DrawMesh(){
    for(size_t mi = 0; mi < this->_numMeshes; mi++){
        _Mesh* mesh = &(this->_meshes[mi]);

        for (size_t ni = 0; ni < mesh->numInstances; ni++){
            _Node* node = &(this->_nodes[mesh->nodeInstances[ni]]);

            for (size_t pi = 0; pi < mesh->numParts; pi++){
                rlPushMatrix();
                    DrawMesh(this->rlMesh[mi].partsMesh[pi], this->mat, Ray_FBX::CopyMatrix(node->geometry_to_world.m));
                rlPopMatrix();
            }
        }
    }
}

void Ray_FBX::_DrawModel(){
    for(size_t mi = 0; mi < this->_numMeshes; mi++){
        _Mesh* mesh = &(this->_meshes[mi]);

        for (size_t ni = 0; ni < mesh->numInstances; ni++){
            _Node* node = &(this->_nodes[mesh->nodeInstances[ni]]);

            for (size_t pi = 0; pi < mesh->numParts; pi++){
                rlPushMatrix();
                    rlMultMatrixf(node->geometry_to_world.m);
                    DrawModel(this->rlMesh[mi].partsModel[pi], {0.0f}, 1.0f, this->rlMesh[mi].color);
                rlPopMatrix();
            }
        }
    }
}

void Ray_FBX::_DrawMeshIgnoreNodes(){
    for(size_t mi = 0; mi < this->_numMeshes; mi++){
        _Mesh* mesh = &(this->_meshes[mi]);
        for (size_t pi = 0; pi < mesh->numParts; pi++){
            rlPushMatrix();
                DrawMesh(this->rlMesh[mi].partsMesh[pi], this->mat, Ray_FBX::IdentityMatrix());
            rlPopMatrix();
        }
    }
}

void Ray_FBX::_DrawModelIgnoreNodes(){
    for(size_t mi = 0; mi < this->_numMeshes; mi++){
        _Mesh* mesh = &(this->_meshes[mi]);
        for (size_t pi = 0; pi < mesh->numParts; pi++){
            rlPushMatrix();
                DrawModel(this->rlMesh[mi].partsModel[pi], {0.0f}, 1.0f, this->rlMesh[mi].color);
            rlPopMatrix();
        }
    }
}

void Ray_FBX::_DrawGLMesh(){
    for(size_t mi = 0; mi < this->_numMeshes; mi++){
        _Mesh* mesh = &(this->_meshes[mi]);

        for (size_t ni = 0; ni < mesh->numInstances; ni++){
            _Node* node = &(this->_nodes[mesh->nodeInstances[ni]]);

            for (size_t pi = 0; pi < mesh->numParts; pi++){

                    glUseProgram(this->shader.id);

                    Vector4 color = ColorNormalize(this->rlMesh[mi].color);
                    glUniform4fv(this->shaderUniforms.color, 1, (float *)&color);

                    Matrix modelViewProjection = MatrixMultiply(rlGetMatrixModelview(), rlGetMatrixProjection());

                    //glUniformMatrix4fv(this->shaderUniforms.mvp, 1, false, MatrixToFloat(modelViewProjection));
                    glUniformMatrix4fv(this->shader.locs[SHADER_LOC_MATRIX_MVP], 1, false, MatrixToFloat(modelViewProjection));
                    glUniformMatrix4fv(this->shaderUniforms.geometry_to_world, 1, false, node->geometry_to_world.m);
                    glUniformMatrix4fv(this->shaderUniforms.normal_to_world, 1, false, node->normal_to_world.m);
                    //glUniformMatrix4fv(this->shaderUniforms.world_to_clip, 1, false, node->geometry_to_node);

                    #define BindBuffer(attribIndex, buffertype, buffer, index, size, type, normalized, stride, pointer) \
                    glEnableVertexAttribArray(attribIndex); \
                    glBindBuffer(buffertype, buffer); \
                    glVertexAttribPointer( \
                        index, \
                        size, type, normalized, stride, pointer \
                    );

                    BindBuffer(
                        0,
                        GL_ARRAY_BUFFER,
                        this->rlMesh[mi].buffers[pi].vertex,
                        //this->shaderUniforms.vertexPos,
                        this->shader.locs[SHADER_LOC_VERTEX_POSITION],
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        nullptr
                    );
                    BindBuffer(
                        1,
                        GL_ARRAY_BUFFER,
                        this->rlMesh[mi].buffers[pi].normal,
                        //this->shaderUniforms.vertexNormal,
                        this->shader.locs[SHADER_LOC_VERTEX_NORMAL],
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        nullptr
                    );
                    BindBuffer(
                        2,
                        GL_ARRAY_BUFFER,
                        this->rlMesh[mi].buffers[pi].uvs,
                        //this->shaderUniforms.vertexUV,
                        this->shader.locs[SHADER_LOC_VERTEX_TEXCOORD01],
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        nullptr
                    );

                    #undef BindBuffer

                    /*glEnableVertexAttribArray(0);
                    glBindBuffer(GL_ARRAY_BUFFER, this->rlMesh[mi].buffers[pi].vertex);
                    glVertexAttribPointer(
                        //this->shader.locs[SHADER_LOC_VERTEX_POSITION],
                        this->shaderUniforms.vertexPos,
                        3, GL_FLOAT, GL_FALSE, 0, nullptr
                    );*/

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->rlMesh[mi].buffers[pi].index);
                    glDrawElements(
                        GL_TRIANGLES,
                        this->_meshes[mi].parts[pi].numIndicies,
                        GL_UNSIGNED_INT,
                        nullptr
                    );

                    glDisableVertexAttribArray(0);
                    glDisableVertexAttribArray(1);
                    glDisableVertexAttribArray(2);

                    //this->rlMesh[mi].partsMesh[pi]
                    /*glBindVertexArray(this->rlMesh[mi].vao[pi]);
                        //glDrawArrays(GL_TRIANGLES, 0, this->_meshes[mi].parts[pi].numVertices);
                        glDrawElements(
                            GL_TRIANGLES,
                            this->_meshes[mi].parts[pi].numIndicies,
                            GL_UNSIGNED_INT,
                            this->_meshes[mi].parts[pi].indicies
                        );
                    glBindVertexArray(0);*/

                    glUseProgram(0);
            }
        }
    }
}

void Ray_FBX::_DrawRLGLMesh(){
    for(size_t mi = 0; mi < this->_numMeshes; mi++){
        _Mesh* mesh = &(this->_meshes[mi]);

        for (size_t ni = 0; ni < mesh->numInstances; ni++){
            _Node* node = &(this->_nodes[mesh->nodeInstances[ni]]);

            for (size_t pi = 0; pi < mesh->numParts; pi++){
                rlPushMatrix();
                    /*rlMultMatrixf(node->geometry_to_world.m);
                    DrawModel(this->rlMesh[mi].partsModel[pi], {0.0f}, 1.0f, this->rlMesh[mi].color);*/
                rlPopMatrix();
            }
        }
    }
}

void Ray_FBX::_DrawMeshBruteForce(){
    for(size_t mi = 0; mi < this->_numMeshes; mi++){
        _Mesh* mesh = &(this->_meshes[mi]);

        for (size_t ni = 0; ni < mesh->numInstances; ni++){
            _Node* node = &(this->_nodes[mesh->nodeInstances[ni]]);
            
            /*if (mesh->skinned){
                for(size_t i=0; i< mesh->numBones; i++){
                    _Node* bone = &(this->_nodes[mesh->boneIndices[i]]);
                    bone->node_to_world;
                    mesh->boneMatrices[i];

                    

                    MatrixMultiply(bone->node_to_world, mesh->boneMatrices[i]);
                }
            }*/

            for (size_t pi = 0; pi < mesh->numParts; pi++){
                rlPushMatrix();
                    rlMultMatrixf(node->geometry_to_world.m);
                    rlMultMatrixf(
                        MatrixToFloat(
                            MatrixScale(0.25f, 0.25f, 0.25f)
                        )
                    );

                    for (int i = 0; i<mesh->parts[pi].numIndicies; i+=3){
                        uint32_t v1i = mesh->parts[pi].indicies[i];
                        uint32_t v2i = mesh->parts[pi].indicies[i+1];
                        uint32_t v3i = mesh->parts[pi].indicies[i+2];

                        DrawTriangle3D(
                            {
                                mesh->parts[pi].vertices[v1i].position[0],
                                mesh->parts[pi].vertices[v1i].position[1],
                                mesh->parts[pi].vertices[v1i].position[2]
                            },
                            {
                                mesh->parts[pi].vertices[v2i].position[0],
                                mesh->parts[pi].vertices[v2i].position[1],
                                mesh->parts[pi].vertices[v2i].position[2]
                            },
                            {
                                mesh->parts[pi].vertices[v3i].position[0],
                                mesh->parts[pi].vertices[v3i].position[1],
                                mesh->parts[pi].vertices[v3i].position[2]
                            },
                            this->rlMesh[mi].color
                        );
                    }
                rlPopMatrix();
            }
        }
    }
}

Matrix Ray_FBX::CopyMatrix(float m[16]){
    Matrix ret;

    #define copyVal(x) ret.m##x = m[x];

    copyVal(0); copyVal(1); copyVal(2); copyVal(3);
    copyVal(4); copyVal(5); copyVal(6); copyVal(7);
    copyVal(8); copyVal(9); copyVal(10); copyVal(11);
    copyVal(12); copyVal(13); copyVal(14); copyVal(15);

    #undef copyVal

    return ret;
}

Matrix Ray_FBX::IdentityMatrix(){
    Matrix ret = {0.0f};

    ret.m0 = ret.m5 = ret.m10 = ret.m15 = 1.0f;

    return ret;
}