#include "Utils.h"

#include "rlgl.h"

#include <math.h>

Camera3D createCamera(){
    Camera3D ret;
    ret.position.x = 5.0f;
    ret.position.y = 5.0f;
    ret.position.z = 5.0f;
    ret.target.x = ret.target.y = ret.target.z = 0.0f;
    ret.up.x = ret.up.z = 0.0f;
    ret.up.y = 1.0f;
    ret.fovy = 90.0f;
    ret.projection = CAMERA_PERSPECTIVE;

    return ret;
}

void Create3DGrid(int slices, int spacing){
    rlPushMatrix();
        rlRotatef(90,1,0,0);
        DrawGrid(slices, spacing);
    rlPopMatrix();

    rlPushMatrix();
        rlRotatef(90,0,1,0);
        DrawGrid(slices, spacing);
    rlPopMatrix();

    rlPushMatrix();
        rlRotatef(90,0,0,1);
        DrawGrid(slices, spacing);
    rlPopMatrix();
}

void RotateAxis(float x, float y, float z){
    rlRotatef(x,1,0,0);
    rlRotatef(y,0,1,0);
    rlRotatef(z,0,0,1);
}

void UpdateCameraControl(float &distance, float &horizontalAngle, float &verticalAngle){
    if (IsKeyPressedRepeat(KEY_LEFT) || IsKeyPressed(KEY_LEFT)){
        horizontalAngle+=1.0f;
    }else if (IsKeyPressedRepeat(KEY_RIGHT) || IsKeyPressed(KEY_RIGHT)) {
        horizontalAngle-=1.0f;
    }

    if (horizontalAngle > 360.0f) horizontalAngle -= 360.0f;
    if (horizontalAngle < 0.0f) horizontalAngle += 360.0f;

    if (IsKeyPressedRepeat(KEY_UP) || IsKeyPressed(KEY_UP)){
        verticalAngle+=1.0f;
    }else if (IsKeyPressedRepeat(KEY_DOWN) || IsKeyPressed(KEY_DOWN)) {
        verticalAngle-=1.0f;
    }

    if (verticalAngle > 360.0f) verticalAngle -= 360.0f;
    if (verticalAngle < 0.0f) verticalAngle += 360.0f;

    if (IsKeyPressedRepeat(KEY_PAGE_UP) || IsKeyPressed(KEY_PAGE_UP)){
        distance-=1.0f;
    }else if (IsKeyPressedRepeat(KEY_PAGE_DOWN) || IsKeyPressed(KEY_PAGE_DOWN)) {
        distance+=1.0f;
    }

    if (distance <= 1.0f) distance = 1.0f;
}

void UpdateCameraView(Camera3D &cam, float distance, float horizontalAngle, float verticalAngle){
    Vector3 target = cam.target;
    float horizontalDistance = distance * cosf(verticalAngle * DEG2RAD);
    cam.position.x = target.x + horizontalDistance * cosf(horizontalAngle * DEG2RAD);
    cam.position.z = target.z + horizontalDistance * sinf(horizontalAngle * DEG2RAD);
    cam.position.y = target.y + distance * sinf(verticalAngle * DEG2RAD);
}