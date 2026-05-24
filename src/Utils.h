#pragma once

#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"

Camera3D createCamera();

void Create3DGrid(int slices = 20, int spacing = 2);

void RotateAxis(float x, float y, float z);

void UpdateCameraControl(float &distance, float &horizontalAngle, float &verticalAngle);

void UpdateCameraView(Camera3D &cam, float distance, float horizontalAngle, float verticalAngle);

#endif