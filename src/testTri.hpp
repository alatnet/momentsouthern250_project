#pragma once
#ifndef TESTTRI_H
#define TESTTRI_H

#include "glm/ext/matrix_float4x4.hpp"

#include <glad/gl.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

void InitTriangleShader();

void ReleaseTriangleShader();

void InitTriangleBuffers();

void ReleaseTriangleBuffers();

void DrawTriangle(glm::mat4x4 mvp);

#endif