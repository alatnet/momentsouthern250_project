#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <ostream>

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include "Utils.h"

//#include "MS250_FBX.h"

#include "Ray_FBX.hpp"

#include "glad.h"


using namespace std;

typedef struct Sim_Mesh {
    float *vertices;
    size_t num_vertices;

    uint16_t* indicies;
    size_t num_indicies;

} Sim_Mesh;

// float* convertQuadToTris(){
//     float* ret = new float[6];

//     return ret;
// }

// void LoadModelData(Mesh &testMesh){
//     ufbx_scene *fbxScene = ufbx_load_file("/home/alatnet/Projects/MomentSouthern250 Project/bin/Cube.fbx", NULL, NULL);
//     //ufbx_scene *fbxScene = ufbx_load_file("/home/alatnet/Projects/MomentSouthern250 Project/bin/primm_rod.fbx", NULL, NULL);

//     if (fbxScene != nullptr) cout<<"Loaded FBX!"<<endl;
//     else{
//         cout<<"Error Loading FBX."<<endl;
//         return;
//     }

//     if (fbxScene != nullptr){

//         cout<<"Number of Meshes: "<<fbxScene->meshes.count<<endl;
//         for(int i=0;i<fbxScene->meshes.count; i++){
//             ufbx_mesh* mesh = fbxScene->meshes[i];

//             cout<<"Mesh "<<i<<":"<<endl<<
//             "- Num Verts: "<<mesh->num_vertices<<endl<<
//             "- Num Tris: "<<mesh->num_triangles<<endl<<
//             "- Num Indices: "<<mesh->num_indices<<endl<<
//             "- Num Faces: "<<mesh->num_faces<<endl;

//             cout<<"Verts:"<<endl;
//             for(int v=0; v<mesh->num_vertices;v++){
//                 cout<<"-Vert "<<v<<": ("<<mesh->vertices[v].x<<", "<<mesh->vertices[v].y<<", "<<mesh->vertices[v].z<<")"<<endl;
//             }

//             cout<<"Indexes: ";
//             for(int index=0;index<mesh->num_indices;index++){
//                 cout<<mesh->vertex_indices[index]<<", ";
//             }
//             cout<<endl;
//         }

//         //https://github.com/ufbx/ufbx/blob/83bc7cf44f76bc8622de63b809a42b5d557cd733/examples/viewer/viewer.c#L401

//         ufbx_mesh* fbxMesh = fbxScene->meshes[0];

//         testMesh.vertexCount = fbxMesh->num_vertices * 3;
//         testMesh.triangleCount = fbxMesh->num_triangles;

//         testMesh.vertices = new float[fbxMesh->num_vertices * 3];
//         testMesh.normals = new float[fbxMesh->num_vertices * 3];
//         testMesh.indices = new unsigned short [fbxMesh->num_indices];

//         for(int i=0, i2=0;i<fbxMesh->num_vertices;i++,i2+=3){
//             testMesh.vertices[i2] = fbxMesh->vertices[i].x;
//             testMesh.vertices[i2+1] = fbxMesh->vertices[i].y;
//             testMesh.vertices[i2+2] = fbxMesh->vertices[i].z;

//             testMesh.normals[i2] = fbxMesh->vertex_normal[i].x;
//             testMesh.normals[i2+1] = fbxMesh->vertex_normal[i].y;
//             testMesh.normals[i2+2] = fbxMesh->vertex_normal[i].z;
//         }

//         for(int i=0;i<fbxMesh->num_indices;i++) testMesh.indices[i] = fbxMesh->vertex_indices[i];

//         /*for(ufbx_mesh** meshI = fbxScene->meshes.begin(); meshI != fbxScene->meshes.end(); meshI++){
//             ufbx_mesh* mesh = *meshI;
//             mesh->vertex_position;
//             mesh->vertex_indices;
//             mesh->vertex_uv;
//             mesh->vertex_color;
//             mesh->num_vertices;
//             mesh->num_indices;
//             mesh->num_triangles;
//         }*/


//     }

//     //free up memory, we dont need the fbxScene;
//     if (fbxScene != nullptr) ufbx_free_scene(fbxScene);
// }

// MS250_FBX *fbxMesh = nullptr;

// struct rlMesh{
//     //Model* partsModel = nullptr;
//     Mesh* partsMesh = nullptr;
//     size_t num_parts = 0;
// } *rlMeshes = nullptr;
// size_t numRLMeshes = 0;

// void LoadData(){
//     fbxMesh = new MS250_FBX("/home/alatnet/Projects/MomentSouthern250 Project/bin/Cube.fbx");
//     //fbxMesh = new MS250_FBX("/home/alatnet/Projects/MomentSouthern250 Project/bin/primm_rod.fbx");

//     if (!fbxMesh->IsLoaded()){
//         delete fbxMesh;
//         fbxMesh = nullptr;
//         return;
//     }
    
//     cout<<"FBX Data:\n"<<fbxMesh->toString()<<endl;

//     numRLMeshes = fbxMesh->numMeshes();
//     rlMeshes = new rlMesh[numRLMeshes];

//     for (int mi = 0; mi<numRLMeshes; mi++){
//         MS250_FBX::_Mesh* mesh = fbxMesh->getMesh(mi);

//         rlMeshes[mi].num_parts = mesh->numParts;
//         //rlMeshes[mi].partsModel = new Model[mesh->numParts];
//         rlMeshes[mi].partsMesh = new Mesh[mesh->numParts];

//         for (int pi = 0; pi < mesh->numParts; pi++){
//             //rlMeshes[i].partsMesh[pi] = new Mesh();
//             //Mesh *pmesh = rlMeshes[i].partsMesh[pi];
//             #define pmesh rlMeshes[mi].partsMesh[pi]

//             pmesh.vertexCount = mesh->parts[pi].numVertices * 3;
//             pmesh.vertices = new float[mesh->parts[pi].numVertices * 3];

//             memcpy(pmesh.vertices, mesh->parts[pi].flatArray.positions, sizeof(float) * 3 * mesh->parts[pi].numVertices);

//             /*for (int i=0, i2=0;i<mesh->parts[pi].numVertices;i++,i2+=3){
//                 pmesh.vertices[i2] = mesh->parts[pi].vertices[i].position[0];
//                 pmesh.vertices[i2+1] = mesh->parts[pi].vertices[i].position[1];
//                 pmesh.vertices[i2+2] = mesh->parts[pi].vertices[i].position[2];
//             }*/

//             pmesh.indices = new unsigned short[mesh->parts[pi].numIndicies];
//             for (int i2=0;i2<mesh->parts[pi].numIndicies; i2++) pmesh.indices[i2] = mesh->parts[pi].indicies[i2];
//             //memcpy(pmesh.indices, (unsigned short*)mesh->parts[pi].indicies, sizeof(unsigned short) * mesh->parts[pi].numIndicies);
//             //std::copy(mesh->parts[pi].indicies, mesh->parts[pi].indicies + mesh->parts[pi].numIndicies, pmesh.indices);

//             //cout<<"Uploading mesh "<<mi<<" - "<<pi<<"."<<endl;
//             UploadMesh(&pmesh, false);
//             //cout<<"Loading mesh to model "<<mi<<" - "<<pi<<"."<<endl;
//             //rlMeshes->partsModel[pi] = LoadModelFromMesh(pmesh);

//             #undef pmesh
//         }
//     }
// }

// void LoadData(){
//     fbxMesh = new MS250_FBX("/home/alatnet/Projects/MomentSouthern250 Project/bin/Cube.fbx");
//     //fbxMesh = new MS250_FBX("/home/alatnet/Projects/MomentSouthern250 Project/bin/primm_rod.fbx");

//     if (!fbxMesh->IsLoaded()){
//         delete fbxMesh;
//         fbxMesh = nullptr;
//         return;
//     }
    
//     cout<<"FBX Data:\n"<<fbxMesh->toString()<<endl;

//     numRLMeshes = fbxMesh->_numMeshes;
//     rlMeshes = new rlMesh[numRLMeshes];

//     for (int i = 0; i<numRLMeshes; i++){
//         rlMeshes[i].num_parts = fbxMesh->_meshes[i].numParts;
//         rlMeshes[i].parts = new Model[fbxMesh->_meshes[i].numParts];
//         rlMeshes[i].partsMesh = new Mesh*[fbxMesh->_meshes[i].numParts];

//         for (int pi = 0; pi < fbxMesh->_meshes[i].numParts; pi++){
//             rlMeshes[i].partsMesh[pi] = new Mesh();
//             Mesh *pmesh = rlMeshes[i].partsMesh[pi];

//             pmesh->vertexCount = fbxMesh->_meshes[i].parts[pi].numVertices;
//             pmesh->vertices = new float[fbxMesh->_meshes[i].parts[pi].numVertices * 3];

//             for (int i3=0, i2=0;i3<fbxMesh->_meshes[i].parts[pi].numVertices;i3++,i2+=3){
//                 pmesh->vertices[i2] = fbxMesh->_meshes[i].parts[pi].vertices[i3].position[0];
//                 pmesh->vertices[i2+1] = fbxMesh->_meshes[i].parts[pi].vertices[i3].position[1];
//                 pmesh->vertices[i2+2] = fbxMesh->_meshes[i].parts[pi].vertices[i3].position[2];
//             }

//             //memcpy(pmesh->vertices, mesh->parts[pi].vertices, mesh->parts[pi].numVertices);

//             pmesh->indices = new unsigned short[fbxMesh->_meshes[i].parts[pi].numIndicies];
//             for (int i2=0;i2<fbxMesh->_meshes[i].parts[pi].numIndicies; i2++) pmesh->indices[i2] = fbxMesh->_meshes[i].parts[pi].indicies[i2];
//             //memcpy(pmesh->indices, mesh->parts[pi].indicies, mesh->parts[pi].numIndicies);

//             cout<<"Uploading mesh "<<i<<" - "<<pi<<"."<<endl;
//             UploadMesh(pmesh, false);
//             rlMeshes->parts[pi] = LoadModelFromMesh(*pmesh);
//         }
//     }
// }

// void UnloadData(){
//     for(size_t i = 0; i < numRLMeshes; i++){
//         // for (size_t pi = 0; pi < rlMeshes[i].num_parts; pi++){
//         //     //UnloadModel(rlMeshes[i].partsModel[pi]);
//         //     UnloadMesh(rlMeshes[i].partsMesh[pi]);
//         //     //delete rlMeshes[i].partsMesh[pi];
//         // }
//         //delete [] rlMeshes[i].partsModel;
//         delete [] rlMeshes[i].partsMesh;
//     }
//     delete [] rlMeshes;
// }

Matrix CopyMatrix(float m[16]){
    Matrix ret;

    #define copyVal(x) ret.m##x = m[x];

    copyVal(0); copyVal(1); copyVal(2); copyVal(3);
    copyVal(4); copyVal(5); copyVal(6); copyVal(7);
    copyVal(8); copyVal(9); copyVal(10); copyVal(11);
    copyVal(12); copyVal(13); copyVal(14); copyVal(15);

    #undef copyVal

    return ret;
}

Matrix IdentityMatrix(){
    Matrix ret = {0.0f};

    ret.m0 = ret.m5 = ret.m10 = ret.m15 = 1.0f;

    return ret;
}

struct TestData{
    Shader shader;
    struct Buffers{
        GLuint vertex = 0;
        GLuint color = 0;
        GLuint normal = 0;
        GLuint uvs = 0;
        GLuint index = 0;
    } buffers;

    GLuint vao = 0;

    struct Mesh{
        float vertex[9] = {
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f
        };

        float color[9] = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f 
        };

        float normal[9] = {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f
        };

        float uv[6] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f
        };

        float indices[3] = { 0, 1, 2 };

    } mesh;

    struct ShaderUniforms{
        int vertexPos = -1;
        int vertexNormal = -1;
        int vertexUV = -1;
        int color = -1;
        int mvp = -1;
        int geometry_to_world = -1;
        int normal_to_world = -1;
        //int world_to_clip;
    } shaderUniforms;
} testData;

void InitTestTriangle(){
    /*#define GenBuffers(buffer, bufferType, size, data, usage) \
    glGenBuffers(1, &(buffer)); \
    glBindBuffer(bufferType, buffer); \
    glBufferData( \
        bufferType, \
        size, \
        data, \
        usage \
    )

    GenBuffers(testData.buffers.vertex, GL_ARRAY_BUFFER, sizeof(float) * 9, testData.mesh.vertex, GL_STATIC_DRAW);
    GenBuffers(testData.buffers.normal, GL_ARRAY_BUFFER, sizeof(float) * 9, testData.mesh.normal, GL_STATIC_DRAW);
    GenBuffers(testData.buffers.uvs, GL_ARRAY_BUFFER, sizeof(float) * 6, testData.mesh.uv, GL_STATIC_DRAW);
    GenBuffers(testData.buffers.index, GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 3, testData.mesh.indices, GL_STATIC_DRAW);

    #undef GenBuffers

    testData.shader = LoadShader("src/test.vs", "src/test.fs");

    if (IsShaderValid(testData.shader)) {
        std::cout<<"TEST TRIANGLE - Shader is Valid."<<std::endl;
    }

    testData.shaderUniforms.vertexPos = GetShaderLocation(testData.shader, "vertexPosition");
    testData.shaderUniforms.vertexNormal = GetShaderLocation(testData.shader, "vertexNormal");
    testData.shaderUniforms.vertexUV = GetShaderLocation(testData.shader, "vertexUV");
    testData.shaderUniforms.color = GetShaderLocation(testData.shader, "color");
    testData.shaderUniforms.mvp = GetShaderLocation(testData.shader, "mvp");
    testData.shaderUniforms.geometry_to_world = GetShaderLocation(testData.shader, "geometry_to_world");
    testData.shaderUniforms.normal_to_world = GetShaderLocation(testData.shader, "normal_to_world");*/

    //testData.shader = LoadShader("src/test.vs", "src/test.fs");

    /*testData.vao = rlLoadVertexArray();
    rlEnableVertexArray(testData.vao);

    testData.buffers.vertex = rlLoadVertexBuffer(testData.mesh.vertex, sizeof(float) * 9, false);
    rlSetVertexAttribute(0, 3, RL_FLOAT, 0, 3 * sizeof(float), 0);

    testData.buffers.color = rlLoadVertexBuffer(testData.mesh.color, sizeof(float) * 9, false);
    rlSetVertexAttribute(1, 3, RL_FLOAT, 0, 3 * sizeof(float), 0);

    //testData.buffers.index =rlLoadVertexBufferElement(testData.mesh.indices, sizeof(testData.mesh.indices), false);

    rlEnableVertexAttribute(0);
    rlEnableVertexAttribute(1);

    rlDisableVertexBuffer();
    rlDisableVertexArray();*/

    testData.shader = LoadShader("src/test.vs", "src/test.fs");

    testData.vao = rlLoadVertexArray();
    rlEnableVertexArray(testData.vao);
    //glGenVertexArrays(1, &(testData.vao));
    //glBindVertexArray(testData.vao);

    testData.buffers.vertex = rlLoadVertexBuffer(&(testData.mesh.vertex), sizeof(float) * 9, false);
    rlEnableVertexBuffer(testData.buffers.vertex);
    rlEnableVertexAttribute(0);
    // glGenBuffers(1, &(testData.buffers.vertex));
    // glBindBuffer(GL_ARRAY_BUFFER, testData.buffers.vertex);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, testData.mesh.vertex, GL_STATIC_DRAW);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    // glEnableVertexAttribArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    testData.buffers.color = rlLoadVertexBuffer(&(testData.mesh.color), sizeof(float) * 9, false);
    rlEnableVertexBuffer(testData.buffers.color);
    rlEnableVertexAttribute(1);
//     glGenBuffers(1, &(testData.buffers.color));
//     glBindBuffer(GL_ARRAY_BUFFER, testData.buffers.color);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, testData.mesh.color, GL_STATIC_DRAW);
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
//     glEnableVertexAttribArray(1);
//     glBindBuffer(GL_ARRAY_BUFFER, 1);
}

void DrawTestTriangle(Camera3D cam){
    /*glUseProgram(testData.shader.id);

    Vector4 color = ColorNormalize(GREEN);
    glUniform4fv(testData.shaderUniforms.color, 1, (float *)&color);

    //Matrix modelViewProjection = MatrixMultiply(rlGetMatrixModelview(), rlGetMatrixProjection());
    
    Matrix modelViewProjection = GetCameraMatrix(cam);

    //glUniformMatrix4fv(this->shaderUniforms.mvp, 1, false, MatrixToFloat(modelViewProjection));
    glUniformMatrix4fv(testData.shader.locs[SHADER_LOC_MATRIX_MVP], 1, false, MatrixToFloat(modelViewProjection));
    //glUniformMatrix4fv(testData.shaderUniforms.geometry_to_world, 1, false, node->geometry_to_world.m);
    //glUniformMatrix4fv(testData.shaderUniforms.normal_to_world, 1, false, node->normal_to_world.m);
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
        testData.buffers.vertex,
        //testData.shaderUniforms.vertexPos,
        testData.shader.locs[SHADER_LOC_VERTEX_POSITION],
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        nullptr
    );
    BindBuffer(
        1,
        GL_ARRAY_BUFFER,
        testData.buffers.normal,
        //testData.shaderUniforms.vertexNormal,
        testData.shader.locs[SHADER_LOC_VERTEX_NORMAL],
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        nullptr
    );
    BindBuffer(
        2,
        GL_ARRAY_BUFFER,
        testData.buffers.uvs,
        //testData.shaderUniforms.vertexUV,
        testData.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01],
        2,
        GL_FLOAT,
        GL_FALSE,
        0,
        nullptr
    );

    #undef BindBuffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, testData.buffers.index);
    glDrawElements(
        GL_TRIANGLES,
        3,
        GL_UNSIGNED_INT,
        nullptr
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glUseProgram(0);*/

    /*rlEnableShader(testData.shader.id);
        rlEnableVertexArray(testData.vao);
            rlDrawVertexArrayElements(0, 3, testData.mesh.indices);
        rlDisableVertexArray();
    rlDisableShader();*/

    // glUseProgram(testData.shader.id);
    // glBindVertexArray(testData.vao);
    // glDrawArrays(GL_TRIANGLES, 0 ,3);
    // glBindVertexArray(0);
    // glUseProgram(0);

    rlEnableShader(testData.shader.id);
    rlEnableVertexArray(testData.vao);
    rlDrawVertexArray(0, 3);
    rlEnableVertexArray(0);
    rlEnableShader(0);
}

void ReleaseTestTriangle(){
    /*glDeleteBuffers(1, &(testData.buffers.vertex));
    glDeleteBuffers(1, &(testData.buffers.normal));
    glDeleteBuffers(1, &(testData.buffers.uvs));
    glDeleteBuffers(1, &(testData.buffers.index));*/

    /*rlUnloadVertexBuffer(testData.buffers.vertex);
    rlUnloadVertexBuffer(testData.buffers.index);
    rlUnloadVertexArray(testData.vao);*/

    glDeleteBuffers(1, &(testData.buffers.vertex));
    glDeleteBuffers(1, &(testData.buffers.color));
    glDeleteBuffers(1, &(testData.vao));
}

int main(int argc, char *argv[]){
    MS250_FBX::debug = true;

    cout<<"Initializing"<<endl;

    Camera3D cam = createCamera();

    InitWindow(800, 600, "basic Window");
    SetTargetFPS(60);

    //Mesh testMesh{};

    //LoadModelData(testMesh);

    //LoadData();

    //UploadMesh(&testMesh, false);
    //Model testModel = LoadModelFromMesh(testMesh);

    float rotatex = 0.0f;
    float rotatey = 0.0f;
    float rotatez = 0.0f;

    float distance=50.0f, horizontalAngle=45.0f, verticalAngle=30.0f;

    Model tModel = LoadModelFromMesh(GenMeshSphere(1, 32, 32));
    Material tMaterial = LoadMaterialDefault();
    tMaterial.maps[0].color = BLUE;

    InitTestTriangle();

    //Ray_FBX *fbxMesh = new Ray_FBX("/home/alatnet/Projects/MomentSouthern250 Project/bin/Cube.fbx");

    //cout<<"FBX Info:"<<endl<<fbxMesh->toString();
    //Ray_FBX *fbxMesh = new Ray_FBX("/home/alatnet/Projects/MomentSouthern250 Project/bin/primm_rod.fbx");

    while (!WindowShouldClose()){
        UpdateCameraControl(distance, horizontalAngle, verticalAngle);
        UpdateCameraView(cam, distance, horizontalAngle, verticalAngle);

        float delta = GetFrameTime();

        rotatex += delta * 10;
        rotatey += delta * 20;
        rotatez += delta * 30;

        if (rotatex >= 360.0f) rotatex -= 360.0f;
        if (rotatey >= 360.0f) rotatey -= 360.0f;
        if (rotatez >= 360.0f) rotatez -= 360.0f;

        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            BeginMode3D(cam);

                Create3DGrid();

                /*rlPushMatrix();
                    DrawModel(tModel, {0.0f, 0.0f, 0.0f}, 1.0f, LIGHTGRAY);
                rlPopMatrix();

                rlPushMatrix();
                    //RotateAxis(rotatex, rotatey, rotatez);
                    DrawCubeWires(
                        {0.0f, 0.0f, 0.0f},
                        2.0f,
                        2.0f,
                        2.0f,
                        RED
                    );
                rlPopMatrix();*/

                /*rlPushMatrix();
                    DrawModel(testModel, {0.0f, 0.0f, 0.0f}, 1.0f, BLUE);
                rlPopMatrix();*/

                //draw the FBX
                // for (size_t mi = 0; mi < fbxMesh->numMeshes(); mi++){
                //     MS250_FBX::_Mesh* mesh = fbxMesh->getMesh(mi);

                //     for (size_t ni = 0; ni < mesh->numInstances; ni++){
                //         MS250_FBX::_Node* node = fbxMesh->getNode(mesh->nodeInstances[ni]);

                //         for (size_t pi = 0; pi < mesh->numParts; pi++){
                //             rlPushMatrix();
                //                 //rlMultMatrixf(node->geometry_to_world.m);
                //                 //DrawModel(rlMeshes[mi].partsModel[pi], {0}, 1.0f, BLUE);
                //                 //DrawModelEx(rlMeshes[mi].partsModel[pi], {0}, {0}, 0, {0}, BLUE);
                //                 //DrawMesh(rlMeshes[mi].partsMesh[pi], tMaterial, CopyMatrix(node->geometry_to_world.m));
                //                 DrawMesh(rlMeshes[mi].partsMesh[pi], tMaterial, IdentityMatrix());
                //             rlPopMatrix();
                //         }
                //     }
                // }

                /*for (size_t mi = 0; mi < fbxMesh->_numMeshes; mi++){
                    for (size_t ni = 0; ni < fbxMesh->_meshes[mi].numInstances; ni++){

                        for (size_t pi = 0; pi < fbxMesh->_meshes[mi].numParts; pi++){
                            rlPushMatrix();
                                rlMultMatrixf(fbxMesh->_nodes[fbxMesh->_meshes[mi].nodeInstances[ni]].geometry_to_world.m);
                                DrawModel(rlMeshes[mi].parts[pi], {0}, 1.0f, BLUE);
                            rlPopMatrix();
                        }
                    }
                }*/

                //fbxMesh->Draw();//rlPushMatrix();

                //rlLoadIdentity();
                //DrawTestTriangle(cam);
                //rlPopMatrix();

                //DrawTriangle3D({0.0f}, {1.0f}, {0.0f, 1.0f, 0.0f}, MAGENTA);

                rlDrawRenderBatchActive();
                DrawTestTriangle(cam);
            EndMode3D();
        EndDrawing();
    }

    //UnloadModel(testModel);
    //UnloadMesh(testMesh);

    //UnloadData();

    UnloadMaterial(tMaterial);

    UnloadModel(tModel);
    
    //if (fbxMesh != nullptr) delete fbxMesh;

    ReleaseTestTriangle();

    CloseWindow();


    return 0;
}