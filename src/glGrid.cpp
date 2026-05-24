#include "glGrid.hpp"
#include "glad/gl.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_uint4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <fstream>
#include <sstream>
#include <string>

std::string readFile(const std::string &filepath);

glGrid::glGrid(int slices){
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec4> indices;

    //create grid data
    for(int j=0; j<=slices; ++j) {
        for(int i=0; i<=slices; ++i) {
            float x = (float)i/(float)slices;
            float y = 0;
            float z = (float)j/(float)slices;
            vertices.push_back(glm::vec3(x, y, z));
        }
    }

    for(int j=0; j<slices; ++j) {
        for(int i=0; i<slices; ++i) {
            int row1 =  j    * (slices+1);
            int row2 = (j+1) * (slices+1);
            indices.push_back(glm::uvec4(row1+i, row1+i+1, row1+i+1, row2+i+1));
            indices.push_back(glm::uvec4(row2+i+1, row2+i, row2+i, row1+i));
        }
    }

    this->indicesLen = indices.size()*4;

    //create grid buffers
    glCreateVertexArrays(1, &(this->vao));

    //create vertex buffer
    glCreateBuffers(1, &(this->vbo));
    glNamedBufferStorage(
        this->vbo,
        vertices.size()*sizeof(glm::vec3),
        glm::value_ptr(vertices[0]),
        GL_DYNAMIC_STORAGE_BIT
    );

    //create indicies buffer
    glCreateBuffers(1, &(this->ibo));
    glNamedBufferStorage(
        this->ibo,
        indices.size() * sizeof(glm::uvec4),
        glm::value_ptr(indices[0]),
        GL_DYNAMIC_STORAGE_BIT
    );

    glVertexArrayVertexBuffer(
        this->vao,
        0,
        this->vbo,
        0,
        sizeof(glm::vec3)
    );

    glVertexArrayElementBuffer(
        this->vao,
        this->ibo
    );

    glEnableVertexArrayAttrib(
        this->vao,
        0 //vertex attribute position
    );
    glVertexArrayAttribFormat(
        this->vao,
        0, //vertex attribute position
        3,
        GL_FLOAT,
        GL_FALSE,
        0
    );
    glVertexArrayAttribBinding(
        this->vao,
        0, //vertex attribute position
        0 
    );

    //deal with shaders
    this->shader.vertex = glCreateShader(GL_VERTEX_SHADER);
    this->shader.fragment = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vsStr = readFile("src/glGrid.vs");
    const char *vs_cstr = vsStr.c_str();
    //int vs_len = vsStr.length();
    glShaderSource(this->shader.vertex, 1, &vs_cstr, nullptr);
    glCompileShader(this->shader.vertex);

    std::string fsStr = readFile("src/glGrid.fs");
    const char *fs_cstr = fsStr.c_str();
    //int fs_len = fsStr.length();
    glShaderSource(this->shader.fragment, 1, &fs_cstr, nullptr);
    glCompileShader(this->shader.fragment);

    this->shader.program = glCreateProgram();
    glAttachShader(this->shader.program, this->shader.vertex);
    glAttachShader(this->shader.program, this->shader.fragment);
    glLinkProgram(this->shader.program);

    //uniforms
    this->shader.uniform.mvp = glGetUniformLocation(this->shader.program, "mvp");
    this->shader.uniform.color = glGetUniformLocation(this->shader.program, "col");

    //attributes
    this->shader.uniform.pos = glGetAttribLocation(this->shader.program, "vPos");
}

glGrid::~glGrid(){
    glDeleteBuffers(1,  &(this->vbo));
    glDeleteBuffers(1, &(this->ibo));
    glDeleteVertexArrays(1, &(this->vao));

    glDeleteProgram(this->shader.program);
    glDeleteShader(this->shader.vertex);
    glDeleteShader(this->shader.fragment);
}

void glGrid::draw(glm::mat4 cam, glm::vec3 color){
    glUseProgram(this->shader.program);

    glUniformMatrix4fv(this->shader.uniform.mvp, 1, GL_FALSE, glm::value_ptr(cam));
    glUniform3fv(this->shader.uniform.color, 1, glm::value_ptr(color));
    
    glBindVertexArray(this->vao);
    glDrawElements(GL_LINES, this->indicesLen, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glUseProgram(0);
}

/*std::string readFile(const std::string &filepath){
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
}*/