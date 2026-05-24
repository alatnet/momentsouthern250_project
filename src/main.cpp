#include "GL_FBX.hpp"

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"

#include <cmath>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

#include <iostream>
#include <ostream>
#include <signal.h>

#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include "testTri.hpp"
#include "glGrid.hpp"

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

float distance = 50.0f;
float rotatex = 0.0f;
float rotatey = 0.0f;
float pivotX = 0.0f;
float pivotY = 0.0f;
float pivotZ = 0.0f;

bool scancode[2000] = {false};

static void key_callback(GLFWwindow* window, int key, int scancodeCB, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS)
        scancode[key] = true;
    else if (action == GLFW_RELEASE)
        scancode[key] = false;
}

#define ROTX_SPD 1.0f;
#define ROTY_SPD 1.0f;

static void inputUpdate(){
    if (scancode[GLFW_KEY_DOWN]){
        rotatey += ROTY_SPD;
        if (rotatey >= 360.0f) rotatey -= 360.0f;
    } else if (scancode[GLFW_KEY_UP]){
        rotatey -= ROTY_SPD;
        if (rotatey <= 0.0f) rotatey += 360.0f;
    }

    if (scancode[GLFW_KEY_LEFT]){
        rotatex += ROTX_SPD;
        if (rotatex >= 360.0f) rotatex -= 360.0f;
    }
    else if (scancode[GLFW_KEY_RIGHT]){
        rotatex -= ROTX_SPD;
        if (rotatex <= 0.0f) rotatex += 360.0f;
    }

    if (scancode[GLFW_KEY_PAGE_UP]){
        distance -= 0.1f;
        if (distance < 1.0f){
            distance = 1.0f;
            std::cout<<"Distance is 1.0f!"<<std::endl;
        }
    }
    else if (scancode[GLFW_KEY_PAGE_DOWN]){
        distance += 0.1f;
    }

    if (scancode[GLFW_KEY_EQUAL]){
        distance -= 1.0f;
        if (distance < 1.0f){
            distance = 1.0f;
            std::cout<<"Distance is 1.0f!"<<std::endl;
        }
    }else if (scancode[GLFW_KEY_MINUS]){
        distance += 1.0f;
    }

    if (scancode[GLFW_KEY_A]){
        pivotX += 1.0f;
    } else if (scancode[GLFW_KEY_D]){
        pivotX -= 1.0f;
    }

    if (scancode[GLFW_KEY_W]){
        pivotY += 1.0f;
    } else if (scancode[GLFW_KEY_S]){
        pivotY -= 1.0f;
    }
    
    if (scancode[GLFW_KEY_Q]){
        pivotZ += 1.0f;
    } else if (scancode[GLFW_KEY_E]){
        pivotZ -= 1.0f;
    }
}

void APIENTRY debug_callback(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; raise(SIGTRAP); break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

glm::mat4 camera(float Translate, glm::vec2 const& Rotate) {
	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 10000.f);
	// glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	// View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	// View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	// //glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	// return Projection * View/* * Model*/;

    return Projection * glm::lookAt(
        glm::vec3(
            pivotX + glm::cos(Rotate.x) * glm::cos(Rotate.y) * Translate,
            pivotY + glm::sin(Rotate.y) * Translate,
            pivotZ + glm::sin(Rotate.x) * glm::cos(Rotate.y) * Translate
        ),
        glm::vec3(pivotX, pivotY, pivotZ),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

double lastTime = 0.0f;
double currentTime = 0.0f;
double getFrameTime(){
    currentTime = glfwGetTime();
    double frametime = currentTime - lastTime;
    lastTime = currentTime;
    return frametime;
}

int main(){
    MS250_FBX::debug = true;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()){
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Test Program", NULL, NULL);
    if (!window){
        glfwTerminate();
        return -1;    
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        glDebugMessageCallback(debug_callback, nullptr);
        std::cout<<"Debug Output Enabled.\n";
    }

    glDebugMessageInsert(
        GL_DEBUG_SOURCE_APPLICATION,
        GL_DEBUG_TYPE_OTHER,
        0,
        GL_DEBUG_SEVERITY_NOTIFICATION,
        -1,
        "Testing Debug Output."
    );

    InitTriangleShader();
    InitTriangleBuffers();

    GL_FBX *fbxMesh = new GL_FBX("/home/alatnet/Projects/MomentSouthern250 Project/bin/Cube Bone Test.fbx");
    //GL_FBX *fbxMesh = new GL_FBX("/home/alatnet/Projects/MomentSouthern250 Project/bin/primm_rod.fbx");
    //GL_FBX *fbxMesh = new GL_FBX("/home/alatnet/Projects/MomentSouthern250 Project/bin/Eri.fbx");

    #define gridSlices 100
    #define gridScale 1000
    glGrid grid(gridSlices);

    glm::mat4 gridMatrices[3] = {
            glm::rotate(
                glm::translate(
                    glm::scale(
                        glm::mat4(1.0f),
                        glm::vec3(gridScale, gridScale, gridScale)
                    ),
                    glm::vec3(-0.5f, 0.5f, 0.0f)
                ),
                glm::radians(90.0f),
                glm::vec3(1.0f, 0.0f, 0.0f)
            ),
        glm::rotate(
            glm::translate(
                glm::scale(
                    glm::mat4(1.0f),
                    glm::vec3(gridScale, gridScale, gridScale)
                ),
                glm::vec3(-0.5f, 0.0f, 0.5f)
            ),
            glm::radians(90.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        ),
            glm::rotate(
                glm::translate(
                    glm::scale(
                        glm::mat4(1.0f),
                        glm::vec3(gridScale, gridScale, gridScale)
                    ),
                    glm::vec3(0.0f, -0.5f, -0.5f)
                ),
                glm::radians(90.0f),
                glm::vec3(0.0f, 0.0f, 1.0f)
            ),
    };

    glm::vec3 gridColors[3] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };

    glGrid camGrid(1);

    //std::cout<<std::endl;
    //std::cout<<fbxMesh->toString()<<std::endl;
    //std::cout<<std::endl;

    float aabb_min[3] = {INFINITY, INFINITY, INFINITY};
    float aabb_max[3]= {-INFINITY, -INFINITY, -INFINITY};
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    while (!glfwWindowShouldClose(window)){
        inputUpdate();

        double frametime = getFrameTime();

        /*rotatex += frametime * 1.0f;
        rotatey += frametime * 1.0f;*/

        /*if (rotatex >= 360.0f) rotatex -= 360.0f;
        if (rotatey >= 360.0f) rotatey -= 360.0f;*/

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glScissor(0,0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        glm::mat4x4 cam = camera(
            distance,
            glm::vec2(
                glm::radians(rotatex),
                glm::radians(rotatey)
            )
        );

        camGrid.draw(
            cam * 
            glm::rotate(
                glm::translate(
                    glm::scale(
                        glm::mat4(1.0f),
                        glm::vec3(1.0f)
                    ),
                    glm::vec3(pivotX-0.5f, pivotY+0.5f, pivotZ)
                ),
                glm::radians(90.0f),
                glm::vec3(1.0f, 0.0f, 0.0f)
            ),
            glm::vec3(0.0f, 1.0f, 1.0f)
        );
        camGrid.draw(
            cam * 
            glm::rotate(
                glm::translate(
                    glm::scale(
                        glm::mat4(1.0f),
                        glm::vec3(1.0f)
                    ),
                    glm::vec3(pivotX-0.5f, pivotY, pivotZ+0.5f)
                ),
                glm::radians(90.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            ),
            glm::vec3(0.0f, 1.0f, 1.0f)
        );
        camGrid.draw(
            cam * 
            glm::rotate(
                glm::translate(
                    glm::scale(
                        glm::mat4(1.0f),
                        glm::vec3(1.0f)
                    ),
                    glm::vec3(pivotX, pivotY-0.5f, pivotZ-0.5f)
                ),
                glm::radians(90.0f),
                glm::vec3(0.0f, 0.0f, 1.0f)
            ),
            glm::vec3(0.0f, 1.0f, 1.0f)
        );

        for(int i=0;i<3;i++)
            grid.draw(cam * gridMatrices[i], gridColors[i]);

        DrawTriangle(cam * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));
        fbxMesh->Update(frametime);
        fbxMesh->Draw(cam * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete fbxMesh;

    ReleaseTriangleBuffers();
    ReleaseTriangleShader();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}