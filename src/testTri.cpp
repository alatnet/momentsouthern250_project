#include "testTri.hpp"

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <GL/glext.h>
#include <glad/gl.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <sstream>
#include <string>
#include <fstream>

#define TriangleSize 1.0f

float triangleVectors[] = {
    0.0f, 0.0f, 0.0f,
    TriangleSize, 0.0f, 0.0f,
    0.0f, TriangleSize, 0.0f,
};
float triangleColors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
};

unsigned int triangleIndices[] = {0,1,2};

struct TriShader{
    GLuint program;
    GLuint vertex;
    GLuint fragment;

    struct Uniforms{
        GLuint pos;
        GLuint col;
        GLuint mvp;
    } uniforms;

    GLuint vao;

    union BufferUnion{
        struct Buffers{
            GLuint pos;
            GLuint col;
            GLuint index;
        } buffers;
        GLuint bufferPtr[3];
    } bufferUnion;
} triShader;

std::string readFile(const std::string &filepath){
    std::ifstream file(filepath);
    if (!file.is_open()){
        std::stringstream s;
        s<< "Failed to open file: " << filepath;
        glDebugMessageInsert(
            GL_DEBUG_SOURCE_APPLICATION,
            GL_DEBUG_TYPE_OTHER,
            0,
            GL_DEBUG_SEVERITY_NOTIFICATION,
            -1,
            s.str().c_str()
        );
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void InitTriangleShader(){
    triShader.vertex = glCreateShader(GL_VERTEX_SHADER);
    triShader.fragment = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vsStr = readFile("src/testTri.vs");
    const char *vs_cstr = vsStr.c_str();
    //int vs_len = vsStr.length();
    glShaderSource(triShader.vertex, 1, &vs_cstr, nullptr);
    glCompileShader(triShader.vertex);

    std::string fsStr = readFile("src/testTri.fs");
    const char *fs_cstr = fsStr.c_str();
    //int fs_len = fsStr.length();
    glShaderSource(triShader.fragment, 1, &fs_cstr, nullptr);
    glCompileShader(triShader.fragment);

    triShader.program = glCreateProgram();
    glAttachShader(triShader.program, triShader.vertex);
    glAttachShader(triShader.program, triShader.fragment);
    glLinkProgram(triShader.program);

    //uniforms
    triShader.uniforms.mvp = glGetUniformLocation(triShader.program, "mvp");

    //attributes
    triShader.uniforms.col = glGetAttribLocation(triShader.program, "vCol");
    triShader.uniforms.pos = glGetAttribLocation(triShader.program, "vPos");
}

void ReleaseTriangleShader(){
    glDeleteProgram(triShader.program);
    glDeleteShader(triShader.vertex);
    glDeleteShader(triShader.fragment);
}

void InitTriangleBuffers(){
    glCreateVertexArrays(1, &(triShader.vao));
    glCreateBuffers(3, triShader.bufferUnion.bufferPtr);
    
    glNamedBufferStorage(triShader.bufferUnion.buffers.pos, sizeof(float) * 9, triangleVectors, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(triShader.bufferUnion.buffers.col, sizeof(float) * 9, triangleColors, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(triShader.bufferUnion.buffers.index, sizeof(unsigned int) * 3, triangleIndices, GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayVertexBuffer(triShader.vao, 0, triShader.bufferUnion.buffers.pos, 0, sizeof(float) * 3);
    glVertexArrayVertexBuffer(triShader.vao, 1, triShader.bufferUnion.buffers.col, 0, sizeof(float) * 3);
    glVertexArrayElementBuffer(triShader.vao, triShader.bufferUnion.buffers.index);

    glEnableVertexArrayAttrib(triShader.vao, triShader.uniforms.pos);
    glEnableVertexArrayAttrib(triShader.vao, triShader.uniforms.col);

    glVertexArrayAttribFormat(triShader.vao,triShader.uniforms.pos, 3, GL_FLOAT, GL_FALSE, 0 );
    glVertexArrayAttribFormat(triShader.vao,triShader.uniforms.col, 3, GL_FLOAT, GL_FALSE, 0 );

    glVertexArrayAttribBinding(triShader.vao, triShader.uniforms.pos, 0);
    glVertexArrayAttribBinding(triShader.vao, triShader.uniforms.col, 1);

}

void ReleaseTriangleBuffers(){
    glDeleteBuffers(3, triShader.bufferUnion.bufferPtr);
    glDeleteVertexArrays(1, &(triShader.vao));
}

void DrawTriangle(glm::mat4x4 mvp){
    glUseProgram(triShader.program);

    glUniformMatrix4fv(triShader.uniforms.mvp, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindVertexArray(triShader.vao);

    glDrawElements(
        GL_TRIANGLES,
        3,
        GL_UNSIGNED_INT,
        0
    );

    glBindVertexArray(0);
    glUseProgram(0);
}